/*
 * voltageRegulator.c
 *
 *  Created on: 6 Jun 2018
 *      Author: 12443166
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

#define UPPERBOUND 9830.1 //3v 3276.7
#define LOWERBOUND 6553.4//2v 6553.4

//Checks to see which channel Number is being worked
int ChannelNumber;

//
channeldata Samples[3];

//pointers to save the number of raises and lowers
int NumofHighers;
int NumofLowers;


//value reset
float dev = 0;
float travelledtime = 0;
float travelledpercentage = 0;
float timeoutperiod = 0;



void BoundsCheck(int16_t VRMS, int channelNb)
{
  if (VRMS > UPPERBOUND || VRMS < LOWERBOUND)
  {
    SignalsSetALarm();
    ChannelNumber = channelNb; //globally defines which channel number Im currently using
//    definitemode();
    inversetimemode();
  }
  else
  {
    PIT1_Enable(false);
    SignalsClearAlarm();
    SignalsClearHigher();
    SignalsClearLower();
  }
}



void definitemode(void)
{

  PIT1_Set(5000, false); //5000ms which is 5 seconds

}

void inversetimemode(void)
{
  PIT1_Set(10, false);

}


void DefiniteCheck(void)
{
  int16_t newVRMS = Samples[ChannelNumber].myVrms;
  if (newVRMS > UPPERBOUND)
  {
    SignalsSetLower();
    NumofLowers++;
  }
  else if (newVRMS < LOWERBOUND)
  {
    SignalsSetHigher();
    NumofHighers++;
  }
  else
  {
    SignalsClearAlarm();
    SignalsClearHigher();
    SignalsClearLower();
  }


}



void InverseCheck(void)
{
  //check to see if dev is over upperbound
  if ((Samples[0].myVrms > UPPERBOUND))
  {
    if (travelledpercentage < 100)  // AND make sure dev is never zero dev != 0
    {
      dev = ((Samples[0].myVrms - UPPERBOUND) /  3276.7);
      timeoutperiod  = (5 / (2 * dev)) * 1000;  //in ms

      travelledtime += 10;
      travelledpercentage = 100 * (travelledtime / timeoutperiod);

    }

    else
    {
      SignalsSetLower();
      NumofLowers++;

      //values reset
      dev = 0;
      travelledtime = 0;
      travelledpercentage = 0;
      timeoutperiod = 0;


    }

  }
  else if ((Samples[0].myVrms < LOWERBOUND))
  {
    if (travelledpercentage < 100)  // AND make sure dev is never zero dev != 0
    {
      dev = ((LOWERBOUND - Samples[0].myVrms) / 3276.7);
      timeoutperiod  = (5 / (2 * dev)) * 1000;  //in ms

      travelledtime += 10;
      travelledpercentage = 100 * (travelledtime / timeoutperiod);

    }

    else
    {
      SignalsSetHigher();
      NumofHighers++;

      //values reset
      dev = 0;
      travelledtime = 0;
      travelledpercentage = 0;
      timeoutperiod = 0;

    }

  }

  else
  {
    SignalsClearAlarm();
    SignalsClearHigher();
    SignalsClearLower();
  }
}






