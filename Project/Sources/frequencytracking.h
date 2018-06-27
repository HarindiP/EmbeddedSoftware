/*
 * frequencytracking.h
 *
 *  Created on: 26 Jun 2018
 *      Author: 12443166
 */

#ifndef SOURCES_FREQUENCYTRACKING_H_
#define SOURCES_FREQUENCYTRACKING_H_

#include "voltageRegulator.h"

void calcminimum(void);
int16_t AdjecentFind(int16_t position);
int32_t Interpolation(int16_t min, int16_t minaux);
void freqtracking(void);

#endif /* SOURCES_FREQUENCYTRACKING_H_ */
