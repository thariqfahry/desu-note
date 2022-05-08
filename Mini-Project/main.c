/**
 * main.c
 * ELEC5620M Mini-Project program entry point
 *
 *  Created on: 29 March 2022
 *      Author: Thariq Fahry (el17mtf)
 *      SID 201158825
 */ 

#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "HPS_Watchdog/HPS_Watchdog.h"
#include "CustomProjectDrivers/AD7843_TouchScreen/AD7843_TouchScreen.h"

#include "DesuNote/desunote.h"

//debug function if initialization fails
void exitOnFail(signed int status, signed int successStatus){
    if (status != successStatus) {
        exit((int)status); //Add breakpoint here to catch failure
    }
}

// Main program entry point
int main(void) {
    //Initialise LCD and touch screen
    exitOnFail(LT24_initialise(0xFF200060,0xFF200080), LT24_SUCCESS);
    exitOnFail(init_touch(0xFF200060), AD7843_SUCCESS);

    // Launch the Desu Note app.
    desunote();
    
    while (1) HPS_ResetWatchdog(); //Just reset the watchdog.
}

