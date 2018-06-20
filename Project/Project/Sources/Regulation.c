/*
 * Regulation.c
 *
 *  Created on: 20 Jun 2018
 *      Author: 13115605
 */


//Sample processing

#include "Regulation.h"
#include "Requirements.h"


#define DEFINITE_TIME 5000000000 //en nanosecondes

TRegulationMode RegMode = DEFINITE_TIME;

//!!!! DEFINIR LES TEMPS EN NANOSECONDES PARCE QUE CE SONT DES INT !!!!

//Write a function to take 16 samples

uint8_t InverseTimer(int16_t deviation, int16_t* ts)
{
  static uint8_t told = *ts;
  uint8_t invTime;
  invTime = (DEFINITE_TIME / (2 * deviation)) * (1 - (*ts / told));
  told = invTime;
  return invTime;
}

void Definite_Timing_Regulation(int16_t* sample)
{
  static uint8_t alarmSet = 0;
  uint16_t Vrms = VRMS(sample);
  if(Vrms > VRMS_MAX)
  {
    if (alarmSet == 1)
    {
      SetLower();
      alarmSet = 0;
    }
    else
    {
      SetAlarm();
      alarmSet = 1;
      OS_TimeDelay(500); //Wait 5s = 500*clock ticks, 1 clock tick = 10ms
    }
  }
  else if (Vrms < VRMS_MIN)
  {
    if (alarmSet == 1)
    {
      SetRaiser();
      alarmSet = 0;
    }
    else
    {
      SetAlarm();
      alarmSet = 1;
      OS_TimeDelay(500); //Wait 5s = 500*clock ticks, 1 clock tick = 10ms
    }
  }
  else
  {
    ClearLower();
    ClearRaiser();
    alarmSet = 0;
  }
}

void Inverse_Timing_Regulation(int16_t* ts)
{
  static uint8_t alarmSet = 0;
  uint16_t Vrms = VRMS(sample);
  uint16_t deviation;
  if(Vrms > VRMS_MAX)
  {
    if (alarmSet == 1)
    {
      SetLower();
      alarmSet = 0;

    }
    else
    {
      SetAlarm();
      uint8_t timeLeft = DEFINITE_TIME;
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
      SetRaiser();
      alarmSet = 0;
    }
    else
    {
      SetAlarm();
      uint8_t timeLeft = DEFINITE_TIME;
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

/*! @brief Treatment Thread
 *
 * */
static void TreatingSampleThread(void* pData)
{
  for(;;)
  {
    OS_SemaphoreWait(SampleTaken,0);
    float vrms = VRMS(FullSample);
    switch (RegMode)
    {
      case DEFINITE_TIME :
        return Definite_Timing_Regulation(FullSample);  //TODO : How do I get FullSample and SignalPeriod here ?
        break;
      case INVERSE_TIME :
        return Inverse_Timing_Regulation(SignalePeriode);
        break;
      default :
        break;
    }
  }
}
