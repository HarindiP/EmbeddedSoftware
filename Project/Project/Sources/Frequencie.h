/*! @file Frequencie.h
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

#ifndef SOURCES_FREQUENCIE_H_
#define SOURCES_FREQUENCIE_H_

#include "types.h"
#include "kiss_fftr.h"
#include <math.h>


////ATTENTION negative values a bit wrong : 2^15/10 values, not (2^15 - 1)/10
//#define VOLT_TO_ANALOG(x) (x*3277)
//#define ANALOG_TO_VOLT(x) (int16_t)(x/3277)

//Signal period
extern float Frequencie_Ts; //in nanosec

/*! @brief Calculate the object of y using linear interpolation with 2 points (x1,y1) and (x2,y2)
 *
 *  @param   y : the image of the object we are looking for
 *  @param   x1 : X value of the first point
 *  @param   y1 : Y value of the first point
 *  @param   x2 : X value of the second point
 *  @param   y2 : Y value of the second point
 *  @return   the object of the y value (float)
 */
float Interpolation( uint16_t y, float x1, float x2, float y1, float y2);

/*! @brief Calculate the average value of a given array
 *
 *  @param   sample : the array we want to calculate the average from
 *  @return   the average valur (int16_t)
 */
int16_t Average(int16_t* const sample);

/*! @brief Calculate the period of the input signal
 *
 *  @param   sampleArray : the array containing 16 samples of the input signal
 *  @param   Ts : a pointer to the supposed signal period
 */
void Frequencie_Tracking(int16_t* const sampleArray, float* Ts);

/*! @brief Calculate the amplitude of the pics of the FFT of the input signal
 *
 *  @param   sampleArray : the array containing 16 samples of the input signal
 *  @param   amplitude : the array where to store the amplitudes calculated
 */
void Frequencie_FFT(int16_t* sampleArray, float* amplitude);


#endif /* SOURCES_FREQUENCIE_H_ */

/*!
 ** @}
 */
