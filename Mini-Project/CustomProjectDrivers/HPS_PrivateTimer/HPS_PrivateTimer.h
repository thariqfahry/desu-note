/*
 * HPS_PrivateTimer.h
 *
 * Driver library for the ARM Cortex-A9 HPS Private Timer.
 * 
 * Created on: 07 March 2022
 *      Author: Thariq Fahry
 * 
 */

#include <stdbool.h>

#define TIMER_SUCCESS       0
#define TIMER_ERRORNOINIT  -1
#define TIMER_BASE_ADDRESS 	0xFFFEC600
#define TIMER_MAXVAL 		0xFFFFFFFF
#define TIMER_FREQUENCY 	225000000

/*
 * countdown
 *
 * Convenience wrapper to countdown to a value in ms and
 * set the interrupt once done. Maximum value = 19000 ms
 *
 * Inputs:
 *      milliseconds:   Value to count down to.
 */
unsigned int countdown(int milliseconds);

/**
 * Timer_Initialise
 *
 * Initialise the timer with all bits of the timer control register set to 0:
 * timer disabled, prescaler 0, automatic reload 0, and ISR 0.
 *
 * Inputs:
 *      base_address:   base memory address of the timer (address of)
 * 		     			the LOAD register.
 */
signed int Timer_Initialise(unsigned int base_address);

/**
 * Timer_isInitialised
 *
 * Check if the Timer has been successfully initialised.
 * 
 * Outputs:
 *      timer_initialised:  boolean representing the initialisation
 *                          state of the timer (1 = success).
 */
bool Timer_isInitialised(void);

/**
 * Timer_SetPrescaler
 *
 * Set the timer prescaler value.
 *
 * Inputs:
 *      prescale_value: prescale value to load into the timer.  
 */
void Timer_SetPrescaler(unsigned int);

/**
 * Timer_SetLoad
 *
 * Set the timer load value to count down from.
 *
 * Inputs:
 *      load_value: load value to set
 */
void Timer_SetLoad(unsigned int);

/**
 * Timer_SetAutomaticReload
 *
 * Set the timer load value to count down from.
 *
 * Inputs:
 *      automatic_reload_flag: boolean representing what to do on timer overflow.
 *                             0 = stop timer
 *                             1 = reload timer with  load value.
 */
void Timer_SetAutomaticReload(bool);

/**
 * Timer_Enable
 *
 * Enables the timer and starts counting down.
 */
void Timer_Enable(void);

/**
 * Timer_ReadValue
 *
 * Reads the current value of the timer.
 * 
 * Outputs:
 *      current_value:  the current value of the timer.
 */
int Timer_ReadValue(void);

/**
 * Timer_ReadInterrupt
 *
 * Reads the current interrupt flag. If high, automatically clears the flag.
 * 
 * Outputs:
 *      isInterrupt:  boolean representing if the interrupt has been triggered.
 */
bool Timer_ReadInterrupt(void);
