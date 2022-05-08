/*
 * HelperFunctions.h
 *
 * Various helper functions for the DE1-SoC.
 *
 *  Created on: Apr 10, 2022
 *      Author: 201158825 Thariq Fahry
 */

#ifndef HELPERFUNCTIONS_H_
#define HELPERFUNCTIONS_H_

#include "../NeuralNetwork/matrix.h"

// Typedef containing a Point struct.
typedef struct {signed short int x;  signed short int y;} Point;

// Required because (ARM?) C89 does not have min/max
// Not a macro due to potential to cause complications
int max(int a, int b);

int min(int a, int b);

float fmaxf(float a, float b);

// Sleep for a number of milliseconds.
void msleep(int milliseconds);

// Clip x to within _min and _max.
int clip(int x, int _min, int _max);

// Check if x is within _min and _max.
bool within(int x, int _min, int _max);

// Draw a rectangle centered at x,y to the display.
void drawRectangle(int x, int y, int rect_width, int rect_height, int colour);

// Draw a rectangle cornered at x,y to the display.
void drawCornerRect(int xleft, int ytop, int rect_width, int rect_height, int colour);

// Constantly print the position of touches recorded on the touch screen.
void print_touches(void);

// Display touch positions of the LCD and clear it when a KEY[0:3] is pressed.
void DrawForever(void);

// Draw a matrix on the LCD.
void drawmat(Matrix* mat, int xleft, int ytop);

// Write a string to the LCD.
void writeString(char* str, int xleft, int ytop, int xright, int ybot, 
int kerning, int linespc, int colour, int bgcolour, int animate_ms);

// Read a single character of handwriting using the neural network and 
// return its char value.
char read_handwriting(int xleft, int ytop, int size);

#endif
