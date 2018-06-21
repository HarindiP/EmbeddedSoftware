/*
 * Regulation.h
 *
 *  Created on: 20 Jun 2018
 *      Author: 13115605
 */

#ifndef SOURCES_REGULATION_H_
#define SOURCES_REGULATION_H_

#include "Requirements.h"
#include "types.h"
#include "OS.h"
#include "Output.h"
#include "Analog.h"
#include "SCP.h"

//Signal period
extern float* Regulation_Ts; //in nanosec
//Array of 16 samples
extern int16_t FullSample[NB_OF_SAMPLE];
//Vrms array
extern float Vrms[3];
//Semaphores
OS_ECB* SampleTaken;
OS_ECB* FullSampleTaken;
//OS_ECB* SampleProcessed;

//Bool signaled if the alarm has been set for more than 5sec
extern bool Regulation_AlarmSet;

float VRMS(int16_t* const sample);

void RAS();

void DefiniteTimingRegulation(int16_t* sample);

float InverseTimer(int16_t deviation, float* ts);

void InverseTimingRegulation(float* ts);

/*! @brief Put an analog sample of the signal in Volt in the sample array
 *
 * @param channelNb the number of the channel to take the sample from
 */
bool TakeSample(int16_t* const sampleArray, int16_t sample);

/*! @brief Thread to take a full sample
 *
 * */
void Regulation_TakeSampleThread(void* pData);

/*! @brief Treatment Thread
 *
 * */
void Regulation_ProcessSampleThread(void* pData);


#endif /* SOURCES_REGULATION_H_ */
