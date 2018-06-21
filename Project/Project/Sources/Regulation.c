/*
 * Regulation.c
 *
 *  Created on: 20 Jun 2018
 *      Author: 13115605
 */


//Sample processing

#include "Regulation.h"
#include <math.h>
#include "PIT.h"
#include "SCP.h"
#include "MK70F12.h"


//Signal period
float* Regulation_Ts; //in ms
//Array of 16 samples
int16_t FullSample[NB_OF_SAMPLE];

//Alarm signal
bool Regulation_AlarmSet = false;


//!!!! DEFINIR LES TEMPS EN NANOSECONDES PARCE QUE CE SONT DES INT !!!!

//STORE VRMS IN AN ARRAY FOR THE 3 CHENNELS FOR SCP TO WORK

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
  Output_ClearLower();
  Output_ClearRaise();
  Output_ClearAlarm();
  PIT1_Enable(false);
  Regulation_AlarmSet = false;
}

void DefiniteTimingRegulation(int16_t* sample)
{
  float Vrms = VRMS(sample);
  if(Vrms > VRMS_MAX)
  {
    if(Regulation_AlarmSet)
    {
      Output_SetLower();
      Regulation_AlarmSet = false;
    }
    else
    {
      Output_SetAlarm();
//      OS_TimeDelay(500); //Wait 5s = 500*clock ticks, 1 clock tick = 10ms
//      PIT1_Enable(false);
      PIT1_Set(DEFINITE_TIME,true);
//      PIT1_Enable(true);
    }
  }
  else if(Vrms < VRMS_MIN)
  {
    if(Regulation_AlarmSet)
    {
      Output_SetRaise();
      Regulation_AlarmSet = false;
    }
    else
    {
      Output_SetAlarm();
//      OS_TimeDelay(500); //Wait 5s = 500*clock ticks, 1 clock tick = 10ms
//      PIT1_Enable(false);
      PIT1_Set(DEFINITE_TIME,true);
//      PIT1_Enable(true);
    }
  }
  else
  {
    RAS();
  }
}

float InverseTimer(int16_t deviation, float* ts)
{
//  uint64_t told = *ts / 16;
//  t_elapsed = (PIT_LDVAL1 - PIT_CVAL1) / 1000;
//  uint64_t invTime;
//  invTime = (DEFINITE_TIME / (2 * deviation)) * (1 - ((*ts / 16) / told));
//  told = invTime;
//  return invTime;
//  return 0;
}

void InverseTimingRegulation(float* ts)
{
  float Vrms = VRMS(FullSample);
  static uint16_t deviation = 0;
  if(Vrms > VRMS_MAX)
  {
    if (Regulation_AlarmSet)
    {
      Output_SetLower();
      Regulation_AlarmSet = false;
    }
    else
    {
      Output_SetAlarm();
      if((Vrms - VRMS_MAX) != deviation)
      {
        deviation = Vrms - VRMS_MAX;
        uint32_t timeLeft = DEFINITE_TIME;
        PIT1_Set(InverseTimer(deviation,ts),true);
      }

//      while (timeLeft > 0)
//      {
//        timeLeft = InverseTimer(deviation,ts);
//        OS_TimeDelay(timeLeft);
        //take 16 new samples
        //recalculate Vrms and deviation
//        PIT1_Enable(false);
//        PIT1_Set(InverseTimer(deviation,ts),true);
//        PIT1_Enable(true);
//      }
    }
  }
  else if (Vrms < VRMS_MIN)
  {
    if (Regulation_AlarmSet)
    {
      Output_SetRaise();
      Regulation_AlarmSet = false;
    }
    else
    {
      deviation = VRMS_MIN - Vrms;
      Output_SetAlarm();
      uint32_t timeLeft = DEFINITE_TIME;
      while (timeLeft > 0)
      {
        timeLeft = InverseTimer(deviation,ts);
        OS_TimeDelay(timeLeft);
        //take 16 new samples
        //recalculate Vrms and deviation
      }
    }
  }
  else
  {
    RAS();
  }
}

/*Should be were Sample is declared*/
bool TakeSample(int16_t* const sampleArray, int16_t sample)
{
  static uint8_t index = 0;
  *(sampleArray+index) = sample;
  index++;
  //wrap index
  if(index == NB_OF_SAMPLE)
  {
    index = 0;
  }
  return true;
}


void Regulation_TakeSampleThread(void* pData)
{
  for(;;)
  {
    OS_SemaphoreWait(SampleTaken,0);

    static int nbSampleTaken = 0;
    nbSampleTaken++;
    if(nbSampleTaken == NB_OF_SAMPLE)
    {
      nbSampleTaken = 0;
      PIT0_Enable(false);
      OS_SemaphoreSignal(FullSampleTaken);
    }
  }
}


void Regulation_ProcessSampleThread(void* pData)
{
  for(;;)
  {
    OS_SemaphoreWait(FullSampleTaken,0);
    float vrms = VRMS(FullSample);
    switch (SCP_RegMode)
    {
      case DEFINITE_TIMER :
        DefiniteTimingRegulation(FullSample);
        //restart PIT to take a new set of sample
        PIT0_Set((uint32_t)(*Regulation_Ts * 1000 / 16),true);
        PIT0_Enable(true);
        break;
      case INVERSE_TIMER :
        return InverseTimingRegulation(Regulation_Ts);
        break;
      default :
        break;
    }
  }
}
