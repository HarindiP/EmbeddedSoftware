/*
 * voltageRegulator.c
 *
 *  Created on: 6 Jun 2018
 *      Author: 13181680
 */

// 1. Three channels which regulate the actual voltage
// 2. Calculate the average of the three channels
// 3. Send the response (send the average??)

#include "types.h"
#include "analog.h"
#include <types.h>

#define UPPERBOUND 3
#define LOWERBOUND 2

int16_t *dataPtr;

void readLoop(void)
{
  int16_t readvalue = 2.5;
  while(readvalue > LOWERBOUND && readvalue < UPPERBOUND)
  {
    (void) Analog_Get(1, dataPtr);
    readvalue = AnalogtoVoltage(*dataPtr);
  }

  if(readvalue < LOWERBOUND || readvalue > UPPERBOUND ) // Smaller than LOWERBOUND --> Assume PIT was initialized in main
  {
    definitemode();
  }

  else // Higher than the UPPERBOUND
  {
      //nothing for now
  }

}


void definitemode(void)
{
  int16_t temp;

  OS_TimeDelay(500);
  (void) Analog_Get(1, dataPtr);
  temp = AnalogtoVoltage(*dataPtr);

  if (temp > UPPERBOUND || temp < LOWERBOUND )
  {
    PIT_Set(1, true); //once one nanosecond passes to the pitISR
  }


  //interupt produced that calls ISR that mkae changes to voltages depending on the boundary limits


}

void inversetimemode(float value)
{
 float dev;
 float delay;
 float timeelpased;
 float progress;
 float remainingprogress;

 if(value > UPPERBOUND)
 {
   dev = value - UPPERBOUND;

   while(progress < 100)
   {
     delay = (5 / (2 * dev));

     progress = (100 * timeelpased / delay);
     remainingprogress = 100 - progress;

   }
 }

 if(value < LOWERBOUND)
 {
   dev = LOWERBOUND - value;

   while(progress < 100)
   {
     delay = (5 / (2 * dev));

     progress = (100 * timeelpased / delay);
     remainingprogress = 100 - progress;

   }
 }

 //check delay every cycle



}




