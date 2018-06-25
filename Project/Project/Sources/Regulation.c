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
bool Regulation_AlarmSet[3] = {false,false,false};
bool Regulation_AlarmReached[3] = {false,false,false};



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

void RAS(uint8_t index)
{
  //Clear Outputs
  Output_ClearLower();
  Output_ClearRaise();
  Output_ClearAlarm();
  //Disable the alarm PIT
  PIT_Enable(index+1, false);
  //Disable the alarm variables
  Regulation_AlarmSet[index] = false;
  Regulation_AlarmReached[index] = false;
  //Turn off green LED
  LEDs_Off(LED_GREEN);
}

void DefiniteTimingRegulation(float* vrms)
{
  for (int i = 0; i < 3; i++)
  {
    if(vrms[i] > VRMS_MAX)
    {
      Output_SetAlarm();
      if(Regulation_AlarmReached[i])
      {
        Output_SetLower();
        SCP_Lowers++;
      }
      else if(!Regulation_AlarmSet[i])
      {
        Regulation_AlarmSet[i] = true;
  //      OS_TimeDelay(500); //Wait 5s = 500*clock ticks, 1 clock tick = 10ms
        PIT_Set(i+1,DEFINITE_TIME,true);
        //Turn on Green LED
        LEDs_On(LED_GREEN);
      }
    }
    else if(vrms[i] < VRMS_MIN)
    {
      Output_SetAlarm();
      if(Regulation_AlarmReached[i])
      {
        Output_SetRaise();
        SCP_Raises++;
      }
      else if(!Regulation_AlarmSet[i])
      {
        Regulation_AlarmSet[i] = true;
  //      OS_TimeDelay(500); //Wait 5s = 500*clock ticks, 1 clock tick = 10ms
        PIT_Set(i+1,DEFINITE_TIME,true);
        //Turn on Green LED
        LEDs_On(LED_GREEN);
      }
    }
    else
    {
      RAS(i);
    }
  }
}


/*BEWARE wrong if moduleClock isnt CPU bus clock*/
uint32_t InverseTimer(uint8_t index, float deviation, bool firstCall)
{
  static uint32_t invTime;
  if(firstCall)
  {
    invTime = (uint32_t) (DEFINITE_TIME / (2 * deviation));
  }
  else
  {
    uint32_t t_elapsed;
    switch (index)
      {
        case 0 :
          t_elapsed = ((PIT_LDVAL1 - PIT_CVAL1 + 1) * 1e3) / CPU_BUS_CLK_HZ;   //time elapsed in ms
          PIT_Enable(1,false);
          break;
        case 1 :
          t_elapsed = ((PIT_LDVAL2 - PIT_CVAL2 + 1) * 1e3) / CPU_BUS_CLK_HZ;   //time elapsed in ms
          PIT_Enable(2,false);
          break;
        case 2 :
          t_elapsed = ((PIT_LDVAL3 - PIT_CVAL3 + 1) * 1e3) / CPU_BUS_CLK_HZ;   //time elapsed in ms
          PIT_Enable(3,false);
          break;
        default :
          break;
    }
    invTime = (uint32_t)((DEFINITE_TIME / (2 * deviation)) * (1 - (t_elapsed / invTime)));
    if(invTime <= 1)
    {
      return 1;
    }
  }
  return invTime;
}

void InverseTimingRegulation(float* vrms)
{
  static float deviation[3] = {0,0,0};
  static bool firstCall[3] = {true, true, true};
  for(int i = 0; i < 3; i++)
  {
    if(*(vrms+i) > VRMS_MAX)
    {
      if(Regulation_AlarmReached[i])
      {
        Output_SetLower();
        SCP_Lowers++;
        firstCall[i] = true;
        deviation[i] = 0;
      }
      else if((*(vrms+i) - VRMS_MAX) != deviation[i])
      {
        deviation[i] = *(vrms+i) - VRMS_MAX;
        Output_SetAlarm();
        Regulation_AlarmSet[i] = true;
        PIT_Set(i+1,InverseTimer(i,deviation[i], firstCall[i]),true);
        //Turn on Green LED
        LEDs_On(LED_GREEN);
        firstCall[i] = false;
      }
    }
    else if (*(vrms+i) < VRMS_MIN)
    {
      if(Regulation_AlarmReached[i])
      {
        Output_SetRaise();
        SCP_Raises++;
        firstCall[i] = true;
        deviation[i] = 0;
      }
      else if((VRMS_MIN - *(vrms+i)) != deviation[i])
      {
        deviation[i] = VRMS_MIN - *(vrms+i);
        Output_SetAlarm();
        Regulation_AlarmSet[i] = true;
        PIT_Set(i,InverseTimer(i,deviation[i], firstCall[i]),true);
        //Turn on Green LED
        LEDs_On(LED_GREEN);
        firstCall[i] = false;
      }
    }
    else
    {
      RAS(i);
      firstCall[i] = true;
    }
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



