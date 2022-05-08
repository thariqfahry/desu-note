/*
 * HPS_PrivateTimer.c
 *
 * Driver library for the ARM Cortex-A9 HPS Private Timer.
 * 
 * Created on: 07 March 2022
 *      Author: Thariq Fahry
 * 
 */

#include "HPS_PrivateTimer.h"

//Driver Base Addresses
volatile unsigned int   *timer_base_ptr     = 0x0;  //0xFFFEC600

//Driver Initialised
bool timer_initialised = false;

//Register Offsets
#define TIMER_LOAD      (0x00/sizeof(unsigned int))
#define TIMER_VALUE     (0x04/sizeof(unsigned int))
#define TIMER_CONTROL   (0x08/sizeof(unsigned int))
#define TIMER_INTERRUPT (0x0C/sizeof(unsigned int))


unsigned int countdown(int milliseconds){
    unsigned int loadvalue = (int)(((float)milliseconds/(float)1000)*TIMER_FREQUENCY);

    if (!Timer_isInitialised()){
        Timer_Initialise(TIMER_BASE_ADDRESS);
        Timer_SetAutomaticReload(false);
    }

    Timer_SetLoad(loadvalue);
    Timer_Enable();

    return (milliseconds/1000)*TIMER_FREQUENCY;
}

//Function to initialise the Timer
signed int Timer_Initialise(unsigned int base_address){
    //Initialise base address pointers
    timer_base_ptr = (unsigned int *)  base_address;
    //Ensure timer initialises to disabled
    timer_base_ptr[TIMER_CONTROL] = 0;
    //Timer now initialised
    timer_initialised = true;
    return TIMER_SUCCESS;
}

bool Timer_isInitialised() 
{
    return timer_initialised;
}

void Timer_SetPrescaler(unsigned int prescale_value)
{
    timer_base_ptr[TIMER_CONTROL] = timer_base_ptr[TIMER_CONTROL] | (prescale_value << 8);
}

void Timer_SetLoad(unsigned int load_value)
{
    timer_base_ptr[TIMER_LOAD] = load_value;
}

void Timer_SetAutomaticReload(bool automatic_reload_flag)
{
    timer_base_ptr[TIMER_CONTROL] = timer_base_ptr[TIMER_CONTROL] | (automatic_reload_flag << 1);
}

void Timer_Enable(void)
{
    timer_base_ptr[TIMER_CONTROL] = timer_base_ptr[TIMER_CONTROL] | (1 << 0);
}

int Timer_ReadValue(void)
{
    return timer_base_ptr[TIMER_VALUE];
}

bool Timer_ReadInterrupt(void)
{
    bool isInterrupt = timer_base_ptr[TIMER_INTERRUPT] & 0x1;
    if (isInterrupt) {
        timer_base_ptr[TIMER_INTERRUPT] = 0x1;
    }
    return isInterrupt;
}
