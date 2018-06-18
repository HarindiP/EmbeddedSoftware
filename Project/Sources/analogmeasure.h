/*
 * analogmeasure.h
 *
 *  Created on: 16 Jun 2018
 *      Author: Harindi Perera
 */

#ifndef SOURCES_ANALOGMEASURE_H_
#define SOURCES_ANALOGMEASURE_H_



void compareMinimum(float value, int position);
/*! @brief Compares the new measured value with the ones already stored
 *
 *  @param value It is the new minimum value
 *  @note It is supposed that the ADC has been initialized
 */
void calculation (int minimumPosition, int16_t minimum, int16_t minimumAux);
/*! @brief Calculates the two lowest values (absolute value) of the array and their position
 *
 *  @note interpoling function
 */
void calculateMinimum(void);

/*! @brief Writes into my array from ADC
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */

void UpdateInput(void);

/*! @brief Writes into my array from ADC
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */

//void Updates(const int8_t channelNb, int16_t *value);



#endif /* SOURCES_ANALOGMEASURE_H_ */
