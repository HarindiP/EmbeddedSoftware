/*
 * calc.h
 *
 *  Created on: 9 Jun 2018
 *      Author: 12443166
 */

#ifndef SOURCES_CALC_H_
#define SOURCES_CALC_H_


/*! @brief It converts voltage to analog format.
 *
 *  @param voltage The voltage measured
 *  @return int16_t The analog value of the input voltage
 */
float calc_VoltageToAnalog(float value);


/*! @brief It converts analog value to voltage format.
 *
 *  @param analogValue The analog value measured
 *  @return int16_t The voltage corresponding to the input analog value
 */
float calc_AnalogToVoltage(float value);

int16_t calc_VRMS(int16_t Sample[]);

#endif /* SOURCES_CALC_H_ */
