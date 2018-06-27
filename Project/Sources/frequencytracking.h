/*
 * frequencytracking.h
 *
 *  Created on: 26 Jun 2018
 *      Author: 12443166
 */

#ifndef SOURCES_FREQUENCYTRACKING_H_
#define SOURCES_FREQUENCYTRACKING_H_

#include "voltageRegulator.h"

/*! @brief calculates the minimum on an array.
 *
 *  @return
 */
void calcminimum(void);
/*! @brief used to get the value passed the x -axis.
 *
 *  @return
 */
int16_t AdjecentFind(int16_t position);
/*! @brief get the zero crossing
 *
 *  @return
 */
int32_t Interpolation(int16_t min, int16_t minaux);
/*! @brief calculates new sampling period.
 *
 *  @return
 */
void freqtracking(void);
/*! @brief
 *
 *  @return
 */
void FFT_Freq(int16_t* samples, float* amplitude);

#endif /* SOURCES_FREQUENCYTRACKING_H_ */
