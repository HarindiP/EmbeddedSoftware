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
#define BITS_PER_VOLT 3276.7 // used to find bits in volts form or the other way around

//local copy from the main
channeldata Samples[3];



//Checks to see which channel Number is being worked //if check channel is channel number use channel number to check the appropriate
int ChannelNumber = 0;

//pointers to save the number of raises and lowers
int NumofHighers;
int NumofLowers;


//value reset
float dev = 0;
float travelledtime = 0;
float travelledpercentage = 0;
float timeoutperiod = 0;

void ValuesReset(void)
{
  float dev = 0;
  float travelledtime = 0;
  float travelledpercentage = 0;
  float timeoutperiod = 0;
}


//create variable to check if channel is in question already havent been checked
bool alreadychecked = false;



void BoundsCheck(int16_t VRMS, int channelNb)
{
  if ((VRMS > UPPERBOUND || VRMS < LOWERBOUND) && (!alreadychecked))
  {
    alreadychecked = true;  //If the channel hadnt been already checked
    SignalsSetALarm();  //Set this goober
    ChannelNumber = channelNb; //globally defines which channel number Im currently using

    // toggles the modes famm!!
    if (TimingMode == DEF_MODE)
    {
      definitemode();
    }
    else if (TimingMode == INV_MODE)
    {
      inversetimemode();
    }

  }


  //makes sure to check the channels that havent been checked
  if (VRMS < UPPERBOUND && VRMS > LOWERBOUND && (ChannelNumber == channelNb) && (alreadychecked))
  {

    alreadychecked = false;
    PIT1_Enable(false);

    // check to see if vrms is inbounds again where i is indicative of channel number
    //if other channels are out of bounds stay yo ass there else clear this bitch
    for (int i = 0; i <3 ; i++)
    {
      if (Samples[i].myVrms > UPPERBOUND || Samples[i].myVrms < LOWERBOUND)
      {
        ValuesReset();
        return;
      }
    }
    // clear all goodbois
  PIT1_Enable(false);
  ValuesReset();
  SignalsClearAll();
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

    //Flash write8 this into memory
  }
  else if (newVRMS < LOWERBOUND)
  {
    SignalsSetHigher();
    NumofHighers++;

    //flash write8 this into memory
  }
  else
  {
    PIT1_Enable(false);
    SignalsClearAll();
  }


}



void InverseCheck(void)
{
  //check to see if dev is over upperbound
  if ((Samples[ChannelNumber].myVrms > UPPERBOUND))
  {
    if (travelledpercentage < 100)  // AND make sure dev is never zero dev != 0
    {
      dev = ((Samples[ChannelNumber].myVrms - UPPERBOUND) /  BITS_PER_VOLT);
      timeoutperiod  = (5 / (2 * dev)) * 1000;  //in ms

      travelledtime += 10;
      travelledpercentage = 100 * (travelledtime / timeoutperiod);

    }

    else
    {
      SignalsSetLower();
      NumofLowers++;

      //values reset all the values used by inverse check since 100% is successfully finished
      ValuesReset();


    }

  }
  else if ((Samples[ChannelNumber].myVrms < LOWERBOUND))
  {
    if (travelledpercentage < 100)  // AND make sure dev is never zero dev != 0
    {
      dev = ((LOWERBOUND - Samples[ChannelNumber].myVrms) / BITS_PER_VOLT);
      timeoutperiod  = (5 / (2 * dev)) * 1000;  //in ms

      travelledtime += 10;
      travelledpercentage = 100 * (travelledtime / timeoutperiod);

    }

    else
    {
      SignalsSetHigher();
      NumofHighers++;

      //values reset all the values used by inverse check since 100% is successfully finished
      ValuesReset();

    }

  }

  else
  {

    SignalsClearAll();
    ValuesReset();
  }
}







