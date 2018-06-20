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

uint16_t VRMS(int16_t* const sample);

int16_t Interpolation( uint16_t y, int16_t x1, int16_t x2, uint16_t y1, uint16_t y2);

int16_t Average(int16_t* const sample);

void FrequencyTracking(int16_t* const sampleArray, uint16_t* Ts);

uint8_t InverseTimer(int16_t deviation, uint16_t* Ts);


/*! @brief Put an analog sample of the signal in Volt in the sample array
 *
 * @param channelNb the number of the channel to take the sample from
 */
bool TakeSample(int16_t* const sampleArray, int16_t sample);


#endif /* SOURCES_FREQUENCIE_H_ */
