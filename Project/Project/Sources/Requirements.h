/*
 * requirements.h
 *
 *  Created on: 9 Jun 2018
 *      Author: 13115605
 */

#ifndef SOURCES_REQUIREMENTS_H_
#define SOURCES_REQUIREMENTS_H_


#define NB_OF_SAMPLE 16
//#define DEFINITE_TIME 5000000000
#define DEFINITE_TIME 5000 //in ms
#define NB_OF_PHASES 3
#define VOLT_OUT_MIN 0
#define VOLT_OUT_MAX 5
#define VRMS_MIN 2
#define VRMS_MAX 3
#define VRMS_AVG 2.5
#define NB_OF_HARMONICS 7
#define SAMPLING_PERIODE 1250000  //in nanosec
#define PRECISION 0.05
#define SIGNAL_PERIOD 20 //in ms
#define FREQ_MAX 52.5
#define FREQ_MIN 47.5

//ATTENTION negative values a bit wrong : 2^15/10 values, not (2^15 - 1)/10
#define VOLT_TO_ANALOG(x) (int16_t)(x*3276.8)
#define ANALOG_TO_VOLT(x) ((float)x/3276.8)


#endif /* SOURCES_REQUIREMENTS_H_ */
