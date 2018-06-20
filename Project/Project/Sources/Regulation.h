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

typedef enum
{
  DEFINITE_TIMER,
  INVERSE_TIMER
} TRegulationMode;

//Signal period
extern uint16_t* Regulation_Ts; //in nanosec
//Array of 16 samples
extern int16_t FullSample[NB_OF_SAMPLE];
//Semaphores
OS_ECB* SampleTaken;
//Semaphores
OS_ECB* SampleProcessed;

float VRMS(int16_t* const sample);

uint8_t InverseTimer(int16_t deviation, int16_t* ts);

void DefiniteTimingRegulation(int16_t* sample);

void InverseTimingRegulation(int16_t* ts);

/*! @brief Put an analog sample of the signal in Volt in the sample array
 *
 * @param channelNb the number of the channel to take the sample from
 */
bool TakeSample(int16_t* const sampleArray, int16_t sample);

/*! @brief Thread to take a full sample
 *
 * */
static void TakingSampleThread(void* pData);

/*! @brief Treatment Thread
 *
 * */
static void TreatingSampleThread(void* pData);


#endif /* SOURCES_REGULATION_H_ */
