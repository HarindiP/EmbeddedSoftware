/*
 * requirements.h
 *
 *  Created on: 9 Jun 2018
 *      Author: 13115605
 */

#ifndef SOURCES_REQUIREMENTS_H_
#define SOURCES_REQUIREMENTS_H_


#define NB_OF_SAMPLE 16
#define DEFINITE_TIME 5
#define NB_OF_PHASES 3
#define VOLT_OUT_MIN 0
#define VOLT_OUT_MAX 5
#define VRMS_MIN 2
#define VRMS_MAX 3
#define NB_OF_HARMONICS 7


//ATTENTION negative values a bit wrong : 2^15/10 values, not (2^15 - 1)/10
#define VOLT_TO_ANALOG(x) (x*3277)
#define ANALOG_TO_VOLT(x) (int16_t)(x/3277)


#endif /* SOURCES_REQUIREMENTS_H_ */
