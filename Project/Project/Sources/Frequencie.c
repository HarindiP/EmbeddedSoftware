/*
 * frequencie.c
 *
 *  Created on: 18/09/06
 *      Author: Coralie K.
 */


#include "Frequencie.h"
#include "Requirements.h"




////find the zero between two values
//int16_t Interpolation(int16_t x1, int16_t x2, uint16_t y1, uint16_t y2)
//{
//  int16_t p = (y2 - y1) / (x2 - x1);
//  int16_t y = ((-y1) / p) + x1;
//  return y;
//}

//Linear interpolation : find the x of a specific image y between two given points
float Interpolation( uint16_t y, float x1, float x2, uint16_t y1, uint16_t y2)
{
  float p = (y2 - y1) / (x2 - x1);
  float x = ((y - y1) / p) + x1;
  return x;
}

//Calculate the average of the set of sample, which is apprximately the DC comp on the sinusoid
int16_t Average(int16_t* const sample)
{
  int16_t average = 0;
  for (int i = 0; i < NB_OF_SAMPLE; i++)
  {
    average += *(sample+i);
  }
  return (average / NB_OF_SAMPLE);
}

//To improve : compare to the average value of the samples, not to 0 (*(Sample+i) - avg)
void FrequencyTracking(int16_t* const sampleArray, float* ts)
{
  uint8_t k = 0;
  float zeros[2];
  while(k < 2)
  {
    for(uint16_t i = 0; i < NB_OF_SAMPLE - 1; i++)
    {
      if(*(sampleArray+i) == 0 )
      {
        zeros[k] = *ts * i / NB_OF_SAMPLE;
        k++;
      }
      if(*(sampleArray+i+1) == 0 )
      {
        zeros[k] = *ts * (i + 1) / NB_OF_SAMPLE;
        k++;
      }
      if (((*(sampleArray+i) - 0)) * ((*(sampleArray+i+1)) - 0) < 0)
      {
        zeros[k] =  Interpolation(0,*ts * i / NB_OF_SAMPLE,*ts * (i + 1) / NB_OF_SAMPLE,*(sampleArray+i),*(sampleArray+i+1));
      }
    }
  }
  if ((zeros[1] - zeros[0]) != (*ts / 2))
  {
    *ts = 2 * (zeros[1] - zeros[0]);
  }
}


