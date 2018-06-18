/*
 * UsefulFunctions.h
 *
 *  Created on: 9 Jun 2018
 *      Author: 13181680
 */

#ifndef SOURCES_USEFULFUNCTIONS_H_
#define SOURCES_USEFULFUNCTIONS_H_


/*! @brief It converts voltage to analog format.
 *
 *  @param voltage The voltage measured
 *  @return int16_t The analog value of the input voltage
 */
float VoltageToAnalog(float value);


/*! @brief It converts analog value to voltage format.
 *
 *  @param analogValue The analog value measured
 *  @return int16_t The voltage corresponding to the input analog value
 */
float AnalogToVoltage(float value);

int16_t VRMS(int16_t Sample[]);

#endif /* SOURCES_USEFULFUNCTIONS_H_ */
