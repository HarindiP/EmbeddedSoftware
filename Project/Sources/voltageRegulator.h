/*
 * voltageRegulator.h
 *
 *  Created on: 18 Jun 2018
 *      Author: 12443166
 */

#ifndef SOURCES_VOLTAGEREGULATOR_H_
#define SOURCES_VOLTAGEREGULATOR_H_

#include "SCP.h"

//define a struct used to navigate all channels on through out the code
typedef struct
{
  int16_t myArray[16];    //Initalises an array of 16 samples
  int16_t myVrms;         //Vrms value of regarding channel
  int myposition; //Current position in the array

}
channeldata;

//Initilising channel data
extern channeldata Samples[3];
extern Tmode SCP_TimingMode;
extern uint8_t SCP_NbHighers;
extern uint8_t SCP_NbLowers;


//Checks to see which channel Number is being worked //if check channel is channel number use channel number to check the appropriate
extern int ChannelNumber;


/*! @brief Resets the initialising variable for inverse check
 *
 *
 */

void ValuesReset(void);

/*! @brief Checks the 3 channels constantly to check if in boundaries before any of the modes set
 *
 *  @param VRMS : The vrms value of a sample
 *  @param channelNb : The coresponding channel number
 *
 *  @note Once on on channel, check the others
 */
void BoundsCheck(int16_t VRMS, int channelNb);

/*! @brief Sets the Pit timer for 5000 ms
 *
 *  @param
 *  @param
 *
 *  @note
 */
void definitemode(void);

/*! @brief Sets the timer to a 10ms
 *
 *  @param
 *  @param
 *
 *  @note
 */
void inversetimemode(void);

/*! @brief Checks the bounds after the timer is done to call Higher and Lower Signals
 *
 *  @param
 *  @param
 *
 *  @note
 */
void DefiniteCheck (void);

/*! @brief Checks the bounds after the timer is done to call Higher and Lower Signals
 *
 *  @param
 *  @param
 *
 *  @note
 */
void InverseCheck(void);



#endif /* SOURCES_VOLTAGEREGULATOR_H_ */
