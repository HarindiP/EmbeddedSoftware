/*! @file
 *
 *  @brief Functions for comparing and selecting minimum values read from ADC
 *
 *  calc.c
 *
 *  @author 12443166
 *  @date 6 Jun 2018
 */

 /*!
  *  @addtogroup UsefulFunctions_module UsefulFunctions module documentation
  *  @{
  */


#include <math.h>
#include <types.h>
#include "OS.h"
#include "MK70F12.h"

//
#define NB_OF_SAMPLE 16
#define BITS_PER_VOLT 3276.7  /* ie. each increment is an increase of 1V will also be in voltage regulator file*/


int16_t calc_VRMS(int16_t Sample[NB_OF_SAMPLE])
{
  float v_rms;

  // what if there are less than 16 samples
  for (int i = 0; i<NB_OF_SAMPLE; i++)
  {
    v_rms += (Sample[i]) * (Sample[i]);
  }

  v_rms = v_rms/16;
  v_rms = sqrt(v_rms);

  //what if vrms is 0

  return (int16_t) v_rms;
}


/*! @brief Converts analog values to voltage by  bits to volts
 *
 *  @param
 *  @note maybe static??
 */
float calc_AnalogToVoltage(int16_t value)
{
//  if (value < 0)
//  {
//    value *= -1;
//  }
  return value / BITS_PER_VOLT;
}


/*! @brief Converts analog values to voltage by  bits to volts
 *
 *  @param
 *  @note maybe static??
 */

int16_t calc_VoltageToAnalog(int16_t value)
{
  int16_t out;
  out = (value * 3267);
  return out;
}


