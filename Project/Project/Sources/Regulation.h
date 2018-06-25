/*
 * Regulation.h
 *
 *  Created on: 20 Jun 2018
 *      Author: 13115605
 */

#ifndef SOURCES_REGULATION_H_
#define SOURCES_REGULATION_H_

#include <math.h>
#include "Cpu.h"
#include "PIT.h"
#include "SCP.h"
#include "MK70F12.h"
#include "LEDs.h"
#include "Requirements.h"
#include "types.h"
#include "OS.h"
#include "Output.h"
#include "Analog.h"
#include "Flash.h"


//Array of 16 samples
extern int16_t Regulation_FullSampleA[NB_OF_SAMPLE];
extern int16_t Regulation_FullSampleB[NB_OF_SAMPLE];
extern int16_t Regulation_FullSampleC[NB_OF_SAMPLE];
//Semaphores
//OS_ECB* SampleTaken;
extern OS_ECB* FullSampleTaken;
//OS_ECB* SampleProcessed;

//Bool signaled if the alarm has been set for more than 5sec
bool Regulation_AlarmSet;
bool Regulation_AlarmReached;

float VRMS(int16_t* const sample);

void RAS();

void DefiniteTimingRegulation(float* vrms);

uint32_t InverseTimer(float deviation, bool firstCall);

void InverseTimingRegulation(float* vrms);

///*! @brief Put an analog sample of the signal in Volt in the sample array
// *
// * @param channelNb the number of the channel to take the sample from
// */
//bool TakeSample(int16_t* const sampleArray, int16_t sample);

///*! @brief Thread to take a full sample
// *
// * */
//void Regulation_TakeSampleThread(void* pData);


#endif /* SOURCES_REGULATION_H_ */
