/*
 * UsefulFunctions.h
 *
 *  Created on: 14 Jun 2018
 *      Author: 12443166
 */

#ifndef SOURCES_USEFULFUNCTIONS_H_
#define SOURCES_USEFULFUNCTIONS_H_


/*Create a struct that holds all possible values for AnalogInput for a channel*/


float AnalogtoVoltage(float x);

float VoltagetoAnalog(float x);

void frequencytracking( ThreaddataStructtype channeldata, uint8_t count);

float timeoffset(float value1 , float value2);



#endif /* SOURCES_USEFULFUNCTIONS_H_ */
