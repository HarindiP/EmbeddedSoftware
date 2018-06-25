/*
 * frequencie.h
 *
 *  Created on: 9 Jun 2018
 *      Author: 13115605
 */

#ifndef SOURCES_FREQUENCIE_H_
#define SOURCES_FREQUENCIE_H_

#include "types.h"


////ATTENTION negative values a bit wrong : 2^15/10 values, not (2^15 - 1)/10
//#define VOLT_TO_ANALOG(x) (x*3277)
//#define ANALOG_TO_VOLT(x) (int16_t)(x/3277)

//Signal period
extern float Frequencie_Ts; //in nanosec


float Interpolation( uint16_t y, float x1, float x2, uint16_t y1, uint16_t y2);

int16_t Average(int16_t* const sample);

void FrequencyTracking(int16_t* const sampleArray, float* Ts);


#endif /* SOURCES_FREQUENCIE_H_ */
