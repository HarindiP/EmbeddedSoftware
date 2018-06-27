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
#include "calc.h"
#include "frequencytracking.h"
#include "voltageRegulator.h"
#include "signals.h"

#define UPPERBOUND 9830.1 //3v 3276.7
#define LOWERBOUND 6553.4//2v 6553.4
#define BITS_PER_VOLT 3276.7 // used to find bits in volts form or the other way around

//local copy from the main
channeldata Samples[3];

//Checks for timing mode
Tmode SCP_TimingMode;

//Checks to see which channel Number is being worked //if check channel is channel number use channel number to check the appropriate
int ChannelNumber = 0;

//Number of Higher
uint8_t SCP_NbHighers;

//Number of Lowers
uint8_t SCP_NbLowers;

//check if channel in question already havent been checked
bool alreadychecked = false;

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


void BoundsCheck(int16_t VRMS, int channelNb)
{
  if ((VRMS > UPPERBOUND || VRMS < LOWERBOUND) && (!alreadychecked))
  {
    alreadychecked = true;  // sets if already hasnt checked
    SignalsSetALarm();
    ChannelNumber = channelNb; //globally defines which channel number Im currently using

    // toggles the modes
    if (SCP_TimingMode == DEF_MODE)
    {
      definitemode();
    }
    else if (SCP_TimingMode == INV_MODE)
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
    for (int i = 0; i <3 ; i++)
    {
      if (Samples[i].myVrms > UPPERBOUND || Samples[i].myVrms < LOWERBOUND)
      {
        ValuesReset();
        return;
      }
    }
    // clear all
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
    SCP_NbLowers++;

    //Flash_Write8((int8_t* NvLowerNbs),SCP_NbLowers);
  }
  else if (newVRMS < LOWERBOUND)
  {
    SignalsSetHigher();
    SCP_NbHighers++;

    //Flash_Write8((int8_t* NvHigherNbs),SCP_NbHighers);
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
    if (travelledpercentage < 100)  // initially travels from 0->100
    {
      dev = ((Samples[ChannelNumber].myVrms - UPPERBOUND) /  BITS_PER_VOLT);
      timeoutperiod  = (5 / (2 * dev)) * 1000;  //calculate delay

      travelledtime += 10;
      travelledpercentage = 100 * (travelledtime / timeoutperiod); //

    }

    else
    {
      SignalsSetLower();
      SCP_NbLowers++;
      //Flash_Write8((int8_t* NvLowerNbs),SCP_NbLowers);

      //values reset all the values used by inverse check since 100% is successfully finished
      ValuesReset();


    }

  }
  else if ((Samples[ChannelNumber].myVrms < LOWERBOUND))
  {
    if (travelledpercentage < 100)  // initially travels from 0->100
    {
      dev = ((LOWERBOUND - Samples[ChannelNumber].myVrms) / BITS_PER_VOLT);
      timeoutperiod  = (5 / (2 * dev)) * 1000;  //calculate delay

      travelledtime += 10;
      travelledpercentage = 100 * (travelledtime / timeoutperiod);

    }

    else
    {
      SignalsSetHigher();
      SCP_NbHighers++;

      //Flash_Write8((int8_t* NvHigherNbs),SCP_NbHighers);

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







