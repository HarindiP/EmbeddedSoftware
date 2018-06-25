/*
 * Regulation.c
 *
 *  Created on: 20 Jun 2018
 *      Author: 13115605
 */


//Sample processing

#include "Regulation.h"

//Array of 16 samples for each chan
int16_t Regulation_FullSampleA[NB_OF_SAMPLE];
int16_t Regulation_FullSampleB[NB_OF_SAMPLE];
int16_t Regulation_FullSampleC[NB_OF_SAMPLE];

//Alarm signal
bool Regulation_AlarmSet = false;
bool Regulation_AlarmReached = false;



//!!!! DEFINIR LES TEMPS EN NANOSECONDES PARCE QUE CE SONT DES INT !!!!

//STORE VRMS IN AN ARRAY FOR THE 3 CHANNELS FOR SCP TO WORK

//Write a function to take 16 samples

float VRMS(int16_t* sample)
{
  float v_rms = 0;
  for (int i = 0; i < NB_OF_SAMPLE; i++)
  {
    v_rms += (*(sample+i)) * (*(sample+i));
  }
  v_rms /= 16;
  //Disable interrupts
  v_rms = (float)sqrt((double)v_rms);
  //Inabe interrupts
  return ANALOG_TO_VOLT(v_rms);
}

void RAS()
{
  //Clear Outputs
  Output_ClearLower();
  Output_ClearRaise();
  Output_ClearAlarm();
  //Disable the alarm PIT
  PIT1_Enable(false);
  //Disable the alarm variables
  Regulation_AlarmSet = false;
  Regulation_AlarmReached = false;
  //Turn off green LED
  LEDs_Off(LED_GREEN);
}

void DefiniteTimingRegulation(float* vrms)
{
  if(vrms[0] > VRMS_MAX || vrms[1] > VRMS_MAX  || vrms[2] > VRMS_MAX)
  {
    if(Regulation_AlarmReached)
    {
      Output_SetLower();
      SCP_Lowers++;
    }
    else if(!Regulation_AlarmSet)
    {
      Output_SetAlarm();
      Regulation_AlarmSet = true;
//      OS_TimeDelay(500); //Wait 5s = 500*clock ticks, 1 clock tick = 10ms
      PIT1_Set(DEFINITE_TIME,true);
      //Turn on Green LED
      LEDs_On(LED_GREEN);
    }
  }
  else if(vrms[0] < VRMS_MIN || vrms[1] < VRMS_MIN || vrms[2] < VRMS_MIN)
  {
    if(Regulation_AlarmReached)
    {
      Output_SetRaise();
      SCP_Raises++;
    }
    else if(!Regulation_AlarmSet)
    {
      Output_SetAlarm();
      Regulation_AlarmSet = true;
//      OS_TimeDelay(500); //Wait 5s = 500*clock ticks, 1 clock tick = 10ms
      PIT1_Set(DEFINITE_TIME,true);
      //Turn on Green LED
      LEDs_On(LED_GREEN);
    }
  }
  else
  {
    RAS();
  }
}


/*BEWARE wrong if noduleClock isnt CPU bus clock*/
uint32_t InverseTimer(float deviation, bool firstCall)
{
  static uint32_t invTime;
  if(firstCall)
  {
    invTime = (uint32_t) (DEFINITE_TIME / (2 * deviation));
  }
  else
  {
    uint32_t t_elapsed = ((PIT_LDVAL1 - PIT_CVAL1 + 1) * 1e3) / CPU_BUS_CLK_HZ;   //time elapsed in ms
    invTime = (uint32_t)((DEFINITE_TIME / (2 * deviation)) * (1 - (t_elapsed / invTime)));
    if(invTime <= 0)
    {
      return 1;
    }
  }
  return invTime;
}

void InverseTimingRegulation(float* vrms)
{
  static bool firstCall = true;
  static float deviation = 0;
  if(*vrms > VRMS_MAX)
  {
    if(Regulation_AlarmReached)
    {
      Output_SetLower();
      SCP_Lowers++;
      firstCall = true;
      deviation = 0;
    }
    else if((*vrms - VRMS_MAX) != deviation)
    {
      deviation = *vrms - VRMS_MAX;
      Output_SetAlarm();
      Regulation_AlarmSet = true;
      PIT1_Set(InverseTimer(deviation, firstCall),true);
      //Turn on Green LED
      LEDs_On(LED_GREEN);
      firstCall = false;
    }
  }
  else if (*vrms < VRMS_MIN)
  {
    if(Regulation_AlarmReached)
    {
      Output_SetRaise();
      SCP_Raises++;
      firstCall = true;
      deviation = 0;
    }
    else if((VRMS_MIN - *vrms) != deviation)
    {
      deviation = VRMS_MIN - *vrms;
      Output_SetAlarm();
      Regulation_AlarmSet = true;
      PIT1_Set(InverseTimer(deviation, firstCall),true);
      //Turn on Green LED
      LEDs_On(LED_GREEN);
      firstCall = false;
    }
  }
  else
  {
    RAS();
    firstCall = true;
  }
}


//bool TakeSample(int16_t* const sampleArray, int16_t sample)
//{
//  static uint8_t index = 0;
//  *(sampleArray+index) = sample;
//  index++;
//  //wrap index
//  if(index == NB_OF_SAMPLE)
//  {
//    index = 0;
//  }
//  return true;
//}


//void Regulation_TakeSampleThread(void* pData)
//{
//  for(;;)
//  {
//    OS_SemaphoreWait(SampleTaken,0);
//
//    static int nbSampleTaken = 0;
//    nbSampleTaken++;
//    if(nbSampleTaken == NB_OF_SAMPLE)
//    {
//      nbSampleTaken = 0;
//      PIT0_Enable(false);
//      OS_SemaphoreSignal(FullSampleTaken);
//    }
//  }
//}



