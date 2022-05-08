/*
 * hardwareaddr.c
 *
 * Some useful hardware addresses that need to be shared globally.
 * 
 * Created on: 20 April 2022
 *      Author: Thariq Fahry
 * 
 */
#include "hardwareaddr.h"

volatile int* KEY_ptr = (int*)0xFF200050;
