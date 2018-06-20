/*
 * Regulation.c
 *
 *  Created on: 20 Jun 2018
 *      Author: 13115605
 */


//Sample processing

#include "Regulation.h"
#include <math.h>


//Signal period
uint16_t* Regulation_Ts; //in nanosec
//Array of 16 samples
int16_t FullSample[NB_OF_SAMPLE];


TRegulationMode RegMode = DEFINITE_TIMER;

//!!!! DEFINIR LES TEMPS EN NANOSECONDES PARCE QUE CE SONT DES INT !!!!

//Write a function to take 16 samples

float VRMS(int16_t* const sample)
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
  return v_rms;
}

uint8_t InverseTimer(int16_t deviation, int16_t* ts)
{
//  uint16_t told = *ts / 16;
//  uint64_t invTime;
//  invTime = (DEFINITE_TIME / (2 * deviation)) * (1 - ((*ts / 16) / told));
//  told = invTime;
//  return invTime;
  return 0;
}

void DefiniteTimingRegulation(int16_t* sample)
{
  static uint8_t alarmSet = 0;
  float Vrms = VRMS(sample);
  if(Vrms > VRMS_MAX)
  {
    if (alarmSet == 1)
    {
      Output_SetLower();
      alarmSet = 0;
    }
    else
    {
      Output_SetAlarm();
      alarmSet = 1;
      OS_TimeDelay(500); //Wait 5s = 500*clock ticks, 1 clock tick = 10ms
    }
  }
  else if (Vrms < VRMS_MIN)
  {
    if (alarmSet == 1)
    {
      Output_SetRaise();
      alarmSet = 0;
    }
    else
    {
      Output_SetAlarm();
      alarmSet = 1;
      OS_TimeDelay(500); //Wait 5s = 500*clock ticks, 1 clock tick = 10ms
    }
  }
  else
  {
    Output_ClearLower();
    Output_ClearRaise();
    alarmSet = 0;
  }
}

void InverseTimingRegulation(int16_t* ts)
{
  static uint8_t alarmSet = 0;
  float Vrms = VRMS(FullSample);
  uint16_t deviation;
  if(Vrms > VRMS_MAX)
  {
    if (alarmSet == 1)
    {
      Output_SetLower();
      alarmSet = 0;

    }
    else
    {
      Output_SetAlarm();
      uint64_t timeLeft = DEFINITE_TIME;
      deviation = Vrms - VRMS_MAX;
      while (timeLeft > 0)
      {
        timeLeft = InverseTimer(deviation,ts);
        OS_TimeDelay(timeLeft);
        //take 16 new samples
        //recalculate Vrms and deviation
      }
      alarmSet = 1;
    }
  }
  else if (Vrms < VRMS_MIN)
  {
    if (alarmSet == 1)
    {
      Output_SetRaise();
      alarmSet = 0;
    }
    else
    {
      Output_SetAlarm();
      uint64_t timeLeft = DEFINITE_TIME;
      deviation = VRMS_MIN - Vrms;
      while (timeLeft > 0)
      {
        timeLeft = InverseTimer(deviation,ts);
        OS_TimeDelay(timeLeft);
        //take 16 new samples
        //recalculate Vrms and deviation
      }
      alarmSet = 1;
    }
  }
  else
  {
    alarmSet = 0;
  }
}

/*Should be were Sample is declared*/
bool TakeSample(int16_t* const sampleArray, int16_t sample)
{
  static uint8_t index = 0;
  *(sampleArray+index) = ANALOG_TO_VOLT(sample);
  index++;
  //wrap index
  if(index == NB_OF_SAMPLE)
  {
    index = 0;
  }
  return true;
}

bool TakeFullSample(uint8_t const channelNb, int16_t* const sampleArray)
{
  for(int i = 0; i < NB_OF_SAMPLE; i++)
  {
    int16_t sample;
    Analog_Get(channelNb,&sample);
    TakeSample(sampleArray,sample);
  }
}

static void TakingSampleThread(void* pData)
{
  for(;;)
  {
    TakeFullSample(0,FullSample);
    OS_SemaphoreSignal(SampleTaken);
  }
}


static void TreatingSampleThread(void* pData)
{
  for(;;)
  {
    OS_SemaphoreWait(SampleTaken,0);
    float vrms = VRMS(FullSample);
    switch (RegMode)
    {
      case DEFINITE_TIMER :
        return DefiniteTimingRegulation(FullSample);
        break;
      case INVERSE_TIMER :
        return InverseTimingRegulation(Regulation_Ts);
        break;
      default :
        break;
    }
    OS_SemaphoreSignal(SampleProcessed);
  }
}
