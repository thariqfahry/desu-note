/*
 * AD7843_TouchScreen.h
 *
 * Driver for the AD7843 Touch Screen ADC.
 *
 *  Created on: Apr 10, 2022
 *      Author: 201158825 Thariq Fahry
 */

#ifndef AD7843_TOUCHSCREEN_H_
#define AD7843_TOUCHSCREEN_H_

#include <stdbool.h>
#include "../../HelperFunctions/HelperFunctions.h"

// Error codes
#define AD7843_SUCCESS       0
#define AD7843_ERRORNOINIT  -1

// Number of reference points for calibration algorithm
#define NCalib 20 

/**
 * init_touch
 *
 * Initialises the touch screen ADC.
 * 
 * Inputs:
 *      pio_base_address: Memory address of the AD7843 ADC PIO.
 * 
 * Outputs:
 *      Integer representing initialiasion success (0 = success.s)
 * 
 */
signed int init_touch(unsigned int pio_base_address);

/**
 * calibrate
 *
 * Calculates and sets calibration parameters for the touch screen against 
 * the LCD using randomly-generated points.
 * 
 */
void calibrate (void);


/**
 * Get_Calibration_Coefficient
 *
 * Calculates the coefficients KX[1-3] and KY[1-3] for the calibration 
 * algorithm. 
 * 
 * This code was copied from Page 9-10 of:
 * https://www.analog.com/media/en/technical-documentation/application-notes/an-1021.pdf.
 * 
 * Inputs:
 *      ReferencePoint: Ncalib by 2 int array containing real coordinates of points.
 *      SamplePoint: Ncalib by 2 int array containing the corresponding raw
 *      coordinates outputted by the touch screen.
 * 
 * Outputs:
 *      Integer representing number of points (Uneeded).
 * 
 */
int Get_Calibration_Coefficient(signed short int ReferencePoint[NCalib][2], signed short int SamplePoint[NCalib][2]);

/**
 * Do_Calibration
 *
 * Calibrates a point.
 * 
 * This code was copied from Page 9-10 of:
 * https://www.analog.com/media/en/technical-documentation/application-notes/an-1021.pdf
 * 
 * Inputs:
 *      Point: Point struct representing an uncalibrated point.
 * 
 * Inputs:
 *      Point struct representing a calibrated point.
 * 
 */
Point Do_Calibration(Point);

/**
 * AD7843_getX
 *
 * Get raw x-value of a touch.
 * 
 * Outputs:
 *      Integer representing raw x-coordinate of the current touch.
 * 
 */
unsigned int AD7843_getX(void);

/**
 * AD7843_getY
 *
 * Get raw y-value of a touch.
 * 
 * Outputs:
 *      Integer representing raw y-coordinate of the current touch.
 * 
 */
unsigned int AD7843_getY(void);

/**
 * AD7843_getCalibXY
 *
 * Get a calibrated Point struct that is clipped to the LCD height and width.
 * 
 * Outputs:
 *      Calibrated Point value of current touch, clipped to LCD height and width.
 * 
 */
Point AD7843_getCalibXY(void);

/**
 * send8bit
 *
 * Sends an 8-bit command word to the AD7843 and get its response.
 * 
 * This code was based on:
 * https://www.topwaydisplay.com/en/blog/how-to-lcd-touch-screen
 * 
 * Outputs:
 *      Serial response from the AD7843.
 * 
 */
unsigned int send8bit(unsigned char command);

/**
 * is_screen_being_touched
 *
 * Check if the screen is currently being touched.
 * 
 * Outputs:
 *      Boolean representing if the screen is currently being touched.
 */
bool is_screen_being_touched(void);

/**
 * wait_for_touch
 *
 * Suspend execution until the screen is touched.
 * 
 * Outputs:
 *      Calibrated Point value of most recent touch.
 */
Point wait_for_touch(void);

/**
 * wait_for_touchrandelease
 *
 * Suspend execution until the screen is touched, then released.
 * 
 * Outputs:
 *      Calibrated Point value of most recent touch.
 */
Point wait_for_touchrandelease(void);

#endif //AD7843_TOUCHSCREEN_H_
