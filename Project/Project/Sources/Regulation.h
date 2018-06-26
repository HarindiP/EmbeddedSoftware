/*! @file Regulation.h
 *
 *  @brief Regulation functions
 *
 *  This contains the functions used for the VRR
 *
 *  @author Coralie
 *  @date 2018-05-20
 */
/*!
 **  @addtogroup Regulation_module Regulation module documentation
 **  @{
 */
/* MODULE Regulation */

#ifndef SOURCES_REGULATION_H_
#define SOURCES_REGULATION_H_

#include <math.h>
#include "Cpu.h"
#include "MK70F12.h"
#include "Requirements.h"
#include "types.h"
#include "OS.h"
#include "Output.h"
#include "Analog.h"
#include "Flash.h"
#include "PIT.h"
#include "SCP.h"


//Array of 16 samples for each chan
int16_t Regulation_FullSampleA[NB_OF_SAMPLE];
int16_t Regulation_FullSampleB[NB_OF_SAMPLE];
int16_t Regulation_FullSampleC[NB_OF_SAMPLE];
//Semaphores
//OS_ECB* SampleTaken;
extern OS_ECB* FullSampleTaken;
//OS_ECB* SampleProcessed;

//Bool signaled if the alarm has been set for more than 5sec
bool Regulation_AlarmSet[3];
extern bool Regulation_AlarmReached[3];


/*! @brief Claculate the True VRMS of the input signal
 *
 *  @param   sample : the set of samples from the signal
 *  @return   the value of the VRMS in Volt (float)
 */
float VRMS(int16_t* const sample);

/*! @brief Everything is fine, clear every alarm signal and raise and lower
 *
 *  @param   index : the channel concerned
 */
void RAS(uint8_t index);

/*! @brief Regulate the Vrms in Definite Time mode
 *
 *  @param   vrms : the value of the current vrms
 */
void Regulation_DefiniteTiming(float* vrms);

/*! @brief Claculate the time to wait
 *
 *  @param   index : the channel concerned
 *  @param   deviation : the deviation of the Vrms
 *  @param   firstCall : a bool indicating if this is called for the first time or not
 *  @return   the time to wait in millisec
 */
uint32_t InverseTimer(uint8_t index, float deviation, bool firstCall);

/*! @brief Regulate the Vrms in Inverse Timing mode
 *
 *  @param   vrms : the value of the current vrms
 */
void Regulation_InverseTiming(float* vrms);

/*! @brief Put an analog sample of the signal in the sample array for each channel
 *
 */
void Regulation_TakeSample();

///*! @brief Thread to take a full sample
// *
// * */
//void Regulation_TakeSampleThread(void* pData);


#endif /* SOURCES_REGULATION_H_ */

/*!
 ** @}
 */
