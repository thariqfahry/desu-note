/*
 * AD7843_TouchScreen.c
 *
 * Driver for the AD7843 Touch Screen ADC.
 *
 *  Created on: Apr 10, 2022
 *      Author: 201158825 Thariq Fahry
 */

#include "AD7843_TouchScreen.h"
#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "HPS_Watchdog/HPS_Watchdog.h"
#include "HPS_usleep/HPS_usleep.h"
#include "../../HelperFunctions/hardwareaddr.h"

#include <stdio.h>
#include <stdlib.h>

int penirq = 29;
int dout   = 28;
int busy   = 27;
int din    = 26;
int cs     = 25;
int dclk   = 24;

// Variables local to driver.
volatile unsigned int* ad7843_pio_ptr = 0x0; //0xFF200060
bool AD7843_Initialised = false;

// Calibration coefficients. Replace these if recalibration is performed.
double KX1 = 0.069215, KX2 = -0.000356, KX3 = -23.386141; 
double KY1 = -0.000425, KY2 = 0.089148, KY3 = -8.874392;

signed int init_touch(unsigned int pio_base_address){
    unsigned int regVal;

    ad7843_pio_ptr = (unsigned int*)pio_base_address;

    regVal = ad7843_pio_ptr[1];

    //set direction bits
    regVal = regVal | ((1ul << din) | (1ul << cs) | (1ul << dclk));// | (1ul << dout)); 
    ad7843_pio_ptr[1] = regVal;

    AD7843_Initialised = true;

    //TODO send and recieve identity to check

    return AD7843_SUCCESS;
}

void calibrate(void){
    unsigned int dx, dy, tx, ty; //display x, display y, touch x, touch y
    int i, getcalib_return;

    signed short int rp[NCalib][2];
    signed short int sp[NCalib][2];

    // Seed the RNG with the Watchdog timer's value so different
    // calibration coordinates are generated each time
    srand(HPS_WatchdogValue());

    for (i = 0; i < NCalib; i++){
        printf("Calibrating %i/%i\n",i , NCalib);

        // generate reference point
        dx = rand() % (LT24_WIDTH - 20) + 10;
        dy = rand() % (LT24_HEIGHT - 20) + 10;
        printf("rp (%i, %i)\n", dx, dy);
        drawRectangle(dx, dy, 5, 5, LT24_BLUE);

        // get sample point
        while(!is_screen_being_touched()) HPS_ResetWatchdog(); //wait for touch
        tx = AD7843_getX();
        ty = AD7843_getY();
        printf("sp = (%i, %i)\n", tx, ty);
        while(is_screen_being_touched()) HPS_ResetWatchdog(); //wait for release

        rp[i][0] = (signed short int)dx;
        rp[i][1] = (signed short int)dy;
    
        sp[i][0] = (signed short int)tx;
        sp[i][1] = (signed short int)ty;

        LT24_clearDisplay(LT24_BLACK);
    }

    getcalib_return = Get_Calibration_Coefficient(rp, sp);

    printf("Calib done; returned %d. Coeffs=\n double KX1 = %f, KX2 = %f, KX3 = %f; \n double KY1 = %f, KY2 = %f, KY3 = %f;\n", getcalib_return ,KX1, KX2, KX3, KY1, KY2, KY3);

    DrawForever();

    // Code to test calibration accuracy post-calibration. Unused.
    //
    // while(1){
    
    // dx = rand() % (LT24_WIDTH - 20) + 10;
    // dy = rand() % (LT24_HEIGHT - 20) + 10;

    // drawRectangle(dx, dy, 5, 5, LT24_BLUE);
    // printf("dx = %i, dy = %i\n", dx, dy);

    // while(!is_screen_being_touched()) HPS_ResetWatchdog(); //wait for touch
    // tx = AD7843_getX();
    // ty = AD7843_getY();
    // printf("raw  tx = %i, ty = %i\n", tx, ty);
    
    // nxy.x = (signed short int)tx; nxy.y = (signed short int)ty;
    // nxy = Do_Calibration(nxy);
    // printf("norm nx = %i, ny = %i\n", nxy.x, nxy.y);

    // xoffset = (float)dx/(float)nxy.x;
    // yoffset = (float)dy/(float)nxy.y;
    // printf("xacc = %f, yacc = %f\n", xoffset, yoffset);
    // printf("\n");


    // while(is_screen_being_touched()) HPS_ResetWatchdog(); //wait for release
    // LT24_clearDisplay(LT24_BLACK);
    // }
}


int Get_Calibration_Coefficient(signed short int ReferencePoint[NCalib][2], signed short int SamplePoint[NCalib][2]) {
    int i;
    int Points = NCalib;
    double a[3], b[3], c[3], d[3], k;
    if (Points < 3) {
        return 0;
    }
    else {
        if (Points == 3) {
            for (i = 0; i < Points; i++) {
                a[i] = (double)(SamplePoint[i][0]);
                b[i] = (double)(SamplePoint[i][1]);
                c[i] = (double)(ReferencePoint[i][0]);
                d[i] = (double)(ReferencePoint[i][1]);
            }
        }
        else if (Points > 3) {
            for (i = 0; i < 3; i++) {
                a[i] = 0;
                b[i] = 0;
                c[i] = 0;
                d[i] = 0;
            }
            for (i = 0; i < Points; i++) {
                a[2] = a[2] + (double)(SamplePoint[i][0]);
                b[2] = b[2] + (double)(SamplePoint[i][1]);
                c[2] = c[2] + (double)(ReferencePoint[i][0]);
                d[2] = d[2] + (double)(ReferencePoint[i][1]);
                a[0] = a[0] + (double)(SamplePoint[i][0]) * (double)(SamplePoint[i][0]);
                a[1] = a[1] + (double)(SamplePoint[i][0]) * (double)(SamplePoint[i][1]);
                b[0] = a[1];
                b[1] = b[1] + (double)(SamplePoint[i][1]) * (double)(SamplePoint[i][1]);
                c[0] = c[0] + (double)(SamplePoint[i][0]) * (double)(ReferencePoint[i][0]);
                c[1] = c[1] + (double)(SamplePoint[i][1]) * (double)(ReferencePoint[i][0]);
                d[0] = d[0] + (double)(SamplePoint[i][0]) * (double)(ReferencePoint[i][1]);
                d[1] = d[1] + (double)(SamplePoint[i][1]) * (double)(ReferencePoint[i][1]);
            }
            a[0] = a[0] / a[2];
            a[1] = a[1] / b[2];
            b[0] = b[0] / a[2];
            b[1] = b[1] / b[2];
            c[0] = c[0] / a[2];
            c[1] = c[1] / b[2];
            d[0] = d[0] / a[2];
            d[1] = d[1] / b[2];
            a[2] = a[2] / Points;
            b[2] = b[2] / Points;
            c[2] = c[2] / Points;
            d[2] = d[2] / Points;
        }
        k = (a[0] - a[2]) * (b[1] - b[2]) - (a[1] - a[2]) * (b[0] - b[2]);
        KX1 = ((c[0] - c[2]) * (b[1] - b[2]) - (c[1] - c[2]) * (b[0] - b[2])) / k;
        KX2 = ((c[1] - c[2]) * (a[0] - a[2]) - (c[0] - c[2]) * (a[1] - a[2])) / k;
        KX3 = (b[0] * (a[2] * c[1] - a[1] * c[2]) + b[1] * (a[0] * c[2] - a[2] * c[0]) + b[2] * (a[1] * c[0] -
            a[0] * c[1])) / k;
        KY1 = ((d[0] - d[2]) * (b[1] - b[2]) - (d[1] - d[2]) * (b[0] - b[2])) / k;
        KY2 = ((d[1] - d[2]) * (a[0] - a[2]) - (d[0] - d[2]) * (a[1] - a[2])) / k;
        KY3 = (b[0] * (a[2] * d[1] - a[1] * d[2]) + b[1] * (a[0] * d[2] - a[2] * d[0]) + b[2] * (a[1] * d[0] -
            a[0] * d[1])) / k;
        return Points;
    }
}

// Calibrate a point
Point Do_Calibration(Point ucp) {
    ucp.x = (signed short int)(KX1 * (ucp.x) + KX2 * (ucp.y) + KX3 + 0.5);
    ucp.y = (signed short int)(KY1 * (ucp.x) + KY2 * (ucp.y) + KY3 + 0.5);
    return ucp;
}

unsigned int AD7843_getX(void){
    return send8bit(0xd0); //LR
}

unsigned int AD7843_getY(void){
    return send8bit(0x90); //UD
}

Point AD7843_getCalibXY(void){
    Point ucp; //uncalibrated point
    ucp.x = (signed short int)AD7843_getX();
    ucp.y = (signed short int)AD7843_getY();
    ucp = Do_Calibration(ucp);

    // Ensure the point is positive and does not exceed  the LCD's width  
    // and height
    ucp.x = min(ucp.x, LT24_WIDTH-1);
    ucp.x = max(ucp.x, 0);

    ucp.y = min(ucp.y, LT24_HEIGHT-1);
    ucp.y = max(ucp.y, 0);

    return ucp;
}


unsigned int send8bit(unsigned char command){
	int i;
	unsigned char temp = command;
    unsigned int response = 0;

    // Ensure we always select 12-bit mode.
    temp &= ~(1ul << 3);

    *ad7843_pio_ptr |= 1ul << dout;
    *ad7843_pio_ptr &= ~(1ul << dclk);
    *ad7843_pio_ptr &= ~(1ul << cs);

    // write the ad7843_pio_ptr word
    for (i = 0;i < 8;i++){
        if (temp & (1 << 7))
        {*ad7843_pio_ptr |= 1ul << din;} else
        {*ad7843_pio_ptr &= ~(1ul << din);}

        // toggle clock
        *ad7843_pio_ptr |= 1ul << dclk; *ad7843_pio_ptr &= ~(1ul << dclk);

        temp = temp << 1;
        usleep(100);
    }

    usleep(1000);
    for (i = 0;i < 12;i++){
        response = response << 1;

        // toggle clock
        *ad7843_pio_ptr |= 1ul << dclk; *ad7843_pio_ptr &= ~(1ul << dclk);

        if ((*ad7843_pio_ptr >> dout) & 1)
        {response |=  1ul;} else
        {response &= ~1ul;}
    }

    *ad7843_pio_ptr |= 1ul << cs;

    for (i = 0;i < 4;i++){
        *ad7843_pio_ptr |= 1ul << dclk; *ad7843_pio_ptr &= ~(1ul << dclk);
    }

    return (response);
}

bool is_screen_being_touched(void){
	return (!((*ad7843_pio_ptr >> penirq) & 1));
}

Point wait_for_touch(void){
    Point touchpos;
    
    //wait for touch
    while (!is_screen_being_touched()) HPS_ResetWatchdog();
    touchpos =  AD7843_getCalibXY();
    return touchpos;
}

Point wait_for_touchrandelease(void){
    Point touchpos;
    touchpos = wait_for_touch();

    //wait for release
    while (is_screen_being_touched()) HPS_ResetWatchdog();
    return touchpos;
}
