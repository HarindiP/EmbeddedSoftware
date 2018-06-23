/*
 * output.c
 *
 *  Created on: 9 Jun 2018
 *      Author: 13115605
 */

#include "analog.h"
#include "Requirements.h"
#include "Output.h"



void Output_SetAlarm()
{
  Analog_Put(2,VOLT_TO_ANALOG(OUTPUT_HIGH));
}

void Output_ClearAlarm()
{
  Analog_Put(2,VOLT_TO_ANALOG(OUTPUT_LOW));
}

void Output_SetRaise()
{
  Analog_Put(0,VOLT_TO_ANALOG(OUTPUT_HIGH));
}

void Output_ClearRaise()
{
  Analog_Put(0,VOLT_TO_ANALOG(OUTPUT_LOW));
}

void Output_SetLower()
{
  Analog_Put(1,VOLT_TO_ANALOG(OUTPUT_HIGH));
}

void Output_ClearLower()
{
  Analog_Put(1,VOLT_TO_ANALOG(OUTPUT_LOW));
}
