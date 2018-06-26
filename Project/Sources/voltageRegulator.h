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


//Checks to see which channel Number is being worked //if check channel is channel number use channel number to check the appropriate
extern int ChannelNumber;


void ValuesReset(void);

void BoundsCheck(int16_t VRMS, int channelNb);

void definitemode(void);

void inversetimemode(void);

void DefiniteCheck (void);

void InverseCheck(void);



#endif /* SOURCES_VOLTAGEREGULATOR_H_ */
