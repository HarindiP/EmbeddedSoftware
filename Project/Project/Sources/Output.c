/*
 * output.c
 *
 *  Created on: 9 Jun 2018
 *      Author: 13115605
 */

#include "analog.h"
#include "Requirements.h"


void Output_SetAlarm()
{
  Analog_Put(2,VOLT_TO_ANALOG(5));
}

void Output_ClearAlarm()
{
  Analog_Put(2,VOLT_TO_ANALOG(0));
}

void Output_SetRaise()
{
  Analog_Put(0,VOLT_TO_ANALOG(5));
}

void Output_ClearRaise()
{
  Analog_Put(0,VOLT_TO_ANALOG(0));
}

void Output_SetALower()
{
  Analog_Put(1,VOLT_TO_ANALOG(5));
}

void Output_ClearLower()
{
  Analog_Put(1,VOLT_TO_ANALOG(0));
}
