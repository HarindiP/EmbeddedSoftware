/*! @file Frequencie.c
 *
 *  @brief Frequencie functions
 *
 *  This contains the functions used to track the signals frequency and to calculate the FFT.
 *
 *  @author Coralie
 *  @date 2018-05-20
 */
/*!
 **  @addtogroup Frequencie_module Frequencie module documentation
 **  @{
 */
/* MODULE Frequencie */


#include "Frequencie.h"
#include "Requirements.h"


//Signal period
float Frequencie_Ts; //in ms

////find the zero between two values
//int16_t Interpolation(int16_t x1, int16_t x2, uint16_t y1, uint16_t y2)
//{
//  int16_t p = (y2 - y1) / (x2 - x1);
//  int16_t y = ((-y1) / p) + x1;
//  return y;
//}

//Linear interpolation : find the x of a specific image y between two given points
float Interpolation( uint16_t y, float x1, float x2, float y1, float y2)
{
  float p = (y2 - y1) / (x2 - x1);
  if(p > 0)
  {
    float x = ((y - y1) / p) + x1;
    return x;
  }
  else if (p < 0)
  {
    float x = ((y - y2) / p) + x2;
    return x;
  }
  else
  {
    return (y2 - y1) / 2;
  }
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
void Frequencie_Tracking(int16_t* const sampleArray, float* ts)
{
  uint8_t k = 0;
  uint16_t i = 0;
  float zeros[2];
  //I am taking the average period calculated on 5 turn to be more accurate;
  static uint8_t tour = 0;
  static float ts_calc = 0;
  //looking for 2 zeros !!! change with average if DC allowed
  while(k < 2 && i < NB_OF_SAMPLE - 1)
  {
    if(sampleArray[i] == 0 )
    {
      zeros[k] = *ts * i / NB_OF_SAMPLE;
      k++;
    }
    if(sampleArray[i+1] == 0 )
    {
      zeros[k] = *ts * (i + 1) / NB_OF_SAMPLE;
      k++;
    }
    if (((sampleArray[i] - 0) * (sampleArray[i+1] - 0)) < 0)
    {
      zeros[k] =  Interpolation(0,*ts * i / NB_OF_SAMPLE,*ts * (i + 1) / NB_OF_SAMPLE,ANALOG_TO_VOLT(sampleArray[i]),ANALOG_TO_VOLT(sampleArray[i+1]));
//      zeros[k] = (ANALOG_TO_VOLT(sampleArray[i]) / (ANALOG_TO_VOLT(sampleArray[i]) - ANALOG_TO_VOLT(sampleArray[i+1]))) + i;
      k++;
    }
    i++;
  }
  tour++;

  //if only 1 zero found, assume default freq
  if(k == 1)
  {
    ts_calc += SIGNAL_PERIOD / 2;
  }
  //if none zeros found, go back to original freq
  else if(k == 0)
  {
    *ts = SIGNAL_PERIOD;
  }
  //everything went well
  else
  {
    ts_calc += (zeros[1] - zeros[0]);
  }
  if (tour == 5)
  {
    ts_calc /= 5;
    if(ts_calc > (1000 / (FREQ_MIN * 2)) || ts_calc < (1000 / (FREQ_MAX * 2)))
    {
      *ts = SIGNAL_PERIOD;
    }
    else
    {
      float error = (ts_calc - (*ts / 2));
      if(error < 0)
      {
        error *= -1;
      }
      if (error > 0.05)
      {
        *ts = 2 * ts_calc;
      }
    }
    ts_calc = 0;
    tour = 0;
  }
}

void Frequencie_FFT(int16_t* sampleArray, float* amplitude)
{
  uint8_t mem[1024];
  size_t sizeMem = sizeof(mem);
  kiss_fftr_cfg cfg = kiss_fftr_alloc(16, 0, mem, &sizeMem);

  kiss_fft_scalar Input[16];
  kiss_fft_cpx Output[9];

  for(int i = 0; i<16; i++)
  {
    Input[i] = ANALOG_TO_VOLT(*(sampleArray+i));
  }
  kiss_fftr( cfg , Input , Output );

  for(int i = 0; i < 9; i++)
  {
    *(amplitude+i) = sqrt(pow(Output[i].r,2) + pow(Output[i].i,2)) ;
  }
}

/*!
 ** @}
 */
