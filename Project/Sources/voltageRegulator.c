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
#include "PIT.h"
#include "OS.h"
#include "UsefulFunctions.h"
#include "analogmeasure.h"
#include "voltageRegulator.h"
#include "signals.h"

#define UPPERBOUND 3
#define LOWERBOUND 2

int16_t *dataPtr;
int16_t checkVRMS;

void readLoop(int16_t VRMS)
{
  if (VRMS > UPPERBOUND || VRMS < LOWERBOUND)

  SignalsSetALarm();
  checkVRMS = VRMS;
  definitemode();
}



void definitemode(void)
{
  int16_t temp;

  OS_TimeDelay(400);
  PIT1_Set(1000000000, true); //once one second passes to the pitISR

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


void DefiniteCheck(void)
{
  int16_t checker = checkVRMS;
  if (checker > UPPERBOUND)
  {
    SignalsSetLower();
  }
  else if (checker < LOWERBOUND)
   {
      SignalsSetHigher();
   }
  else
  {
    SignalsClearAlarm();
  }


}



