/*
 * analogmeasure.h
 *
 *  Created on: 16 Jun 2018
 *      Author: Harindi Perera
 */

#ifndef SOURCES_ANALOGMEASURE_H_
#define SOURCES_ANALOGMEASURE_H_


float myArray[16] = {1,2,3.5,4,6,0.4,7,8,5,4,2,1,-0.7,-0.44,-0.8,-4}; // Array of the total of elements measured
float firstMin; // This is going to be the minimum one (absolute value)
float secondMin; // This is going to be the second minimum one (absolute value)
int firstMinPosition; // Position in myArray of firstMin
int secondMinPosition; // Position in myArray of the secondMin
float period; // deviation
int sampleSize; // Number of values to take into account from the array for measuring
int globalPos; // Position to store new data in the global array

const uint8_t channelNb = 1;
//int16_t* const valuePtr;  //dont need yet???
float * const WritePtr;  //pointer to begining of my array


/*! @brief Compares the new measured value with the ones already stored
 *
 *  @param value It is the new minimum value
 *  @note It is supposed that the ADC has been initialized
 */
void compareMinimum(int16_t value, int position);

/*! @brief Calculates the two lowest values (absolute value) of the array and their position
 *
 *  @note interpoling function
 */
float calculateMinimum(void);

/*! @brief Writes into my array from ADC
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */

void UpdateInput (float * const WritePtr);

/*! @brief Writes into my array from ADC
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */

//void Updates(const int8_t channelNb, int16_t *value);



#endif /* SOURCES_ANALOGMEASURE_H_ */
