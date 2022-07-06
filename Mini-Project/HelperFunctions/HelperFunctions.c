/*
 * HelperFunctions.c
 *
 * Various helper functions for the DE1-SoC.
 *
 *  Created on: Apr 10, 2022
 *      Author: 201158825 Thariq Fahry
 */
#include "HelperFunctions.h"
#include "hardwareaddr.h"

#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "HPS_usleep/HPS_usleep.h"
#include "HPS_Watchdog/HPS_Watchdog.h"
#include "BasicFont/BasicFont.h"

#include "../CustomProjectDrivers/AD7843_TouchScreen/AD7843_TouchScreen.h"
#include "../NeuralNetwork/neuralnet.h"
#include "../CustomProjectDrivers/HPS_PrivateTimer/HPS_PrivateTimer.h"
#include "../DesuNote/uiparams.h"
#include "../Graphics/graphics.h"

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "string.h"

#define HANDWR_INPUT_COLOUR 0x00E4  // Handwriting input stroke.

int max(int a, int b) {
	return (a > b) ? a: b;
}

int min(int a, int b) {
	return (a < b) ? a: b;
}

float fmaxf(float a, float b) {
	return (a > b) ? a: b;
}

void msleep(int milliseconds){
    usleep(1000*milliseconds);
}

int clip(int x, int _min, int _max) {
    return max(_min, min(x, _max));
    }

bool within(int x, int _min, int _max){
    return ((x >= _min) && (x <= _max));
}

void drawRectangle(int x, int y, int rect_width, int rect_height, int colour){
    int i;
    LT24_setWindow(x-(rect_width/2), y-(rect_height/2), rect_width, rect_height);

    for (i=0; i<rect_width*rect_height; i++){
        LT24_write(true, colour);
        }
}

void drawCornerRect(int xleft, int ytop, int rect_width, int rect_height, int colour){
    int i;
    LT24_setWindow(xleft, ytop, rect_width, rect_height);

    for (i=0; i<rect_width*rect_height; i++){
    LT24_write(true, colour);
    }
}

void print_touches(void){
    unsigned int x, y;

    while (1){
        while(!is_screen_being_touched()) HPS_ResetWatchdog(); //wait for touch
        x = send8bit(0x90); //UD
        y = send8bit(0xd0); //LR
        printf("x=%i, y=%i\n",x,y);

        while(is_screen_being_touched()) HPS_ResetWatchdog(); //wait for release
    }
}

void DrawForever(void){
    Point position;
    while(1){
    while(is_screen_being_touched()){
        position = AD7843_getCalibXY();
        drawRectangle(position.x, position.y, 3, 3, LT24_BLUE);
        HPS_ResetWatchdog();
    }
    if (*KEY_ptr) LT24_clearDisplay(LT24_BLACK);
    HPS_ResetWatchdog();
    }
}

void drawmat(Matrix* mat, int xleft, int ytop){
    short colour;
    long i;
    float colourIntensity32;
    
    LT24_setWindow(xleft, ytop, mat->cols, mat->rows);

    for (i = 0; i < mat->cols* mat->rows; i++)
    {
        colourIntensity32 = (float)32*(0.5+*e(mat, 0, i));
        colour = LT24_makeColour(0,(unsigned int)colourIntensity32,0);
        LT24_write(true, colour);
    }
}


void writeString(char* str, int xleft, int ytop, int xright, int ybot, 
                 int kerning, int linespc, int colour, int bgcolour, int animate_ms){
    int i=0,j,k, chcolumn=0, chrow=0;
    char chunk;

    // for each character in the string
    while (str[i] != '\0'){
        if (xleft+(chcolumn+1)*(5+kerning) > xright){
            chrow++; chcolumn=0;
        }
        if (ytop+(chrow+1)*(8+linespc) > ybot){
            printf("not enough space to display string after character[%i], %c\n", i, str[i]);
            return;
        }

        // for each chunk in the character
        for (j = 0; j<5; j++){
            chunk = BF_fontMap[str[i]-' '][j];
            LT24_setWindow(xleft+chcolumn*(5+kerning)+j, ytop+chrow*(8+linespc), 1,8);

            for (k = 0; k < 8; k++){
                if (chunk & 1ul) LT24_write(true, colour);
                else LT24_write(true, bgcolour);
                chunk = chunk >> 1;
            }
        }        
        chcolumn++;
        i++;

        if (animate_ms) usleep(1000*animate_ms);
        HPS_ResetWatchdog();
    }
}


char read_handwriting(int xleft, int ytop, int size){
    Matrix inference_vector = newmat(28, 28, false, -0.5);
    Matrix probability_vector;

    int matrow, matcol;
    Point touchPosition;
    bool touched_at_least_once = false, doneWriting = false;
    char guess_letter;
    
    wait_for_touch();

    while (!(doneWriting) || !(touched_at_least_once)){
        while(is_screen_being_touched()){
            touchPosition = AD7843_getCalibXY();

            if (within(touchPosition.x, xleft, xleft+size) && within(touchPosition.y, ytop ,ytop+size)){   
                touched_at_least_once = true;
                
                drawRectangle(touchPosition.x, touchPosition.y, 2,2, HANDWR_INPUT_COLOUR);

                // Transform touchPosition's x,y to fit inside the inference vector.
                matrow = (int)rintf(((float)(touchPosition.y-ytop)/(float)size)*(float)inference_vector.rows);
                matcol = (int)rintf(((float)(touchPosition.x-xleft)/(float)size)*(float)inference_vector.cols);
        
                set_region(&inference_vector, matrow-1, matcol-1, 2,2,0.5);
                HPS_ResetWatchdog();
            }
        }
        doneWriting = true;
        
        countdown(1500);
        while (!Timer_ReadInterrupt()){
            if (is_screen_being_touched()){
                doneWriting = false;
                break;
            }
            else HPS_ResetWatchdog();
        }
    }
    // Clear the input region.
    LT24_copyFrameBuffer(handwr_field, HANDWR_XLEFT, HANDWR_YTOP, HANDWR_SIZE, HANDWR_SIZE);

    // calculate inference
    // plot inf vec for confirmation
    //drawmat(&inference_vector, 0,0);
    gaussian(&inference_vector);

    // plot gaussed inf vec
    //drawmat(&inference_vector, 28, 0);

    probability_vector = infer(&inference_vector);
    //printmat(&probability_vector, false);
    guess_letter =  argmax2char(argmax(&probability_vector));

    return guess_letter;
}
