/*
 * output.h
 *
 *  Created on: 13 Jun 2018
 *      Author: 12443166
 */

#ifndef SOURCES_OUTPUT_H_
#define SOURCES_OUTPUT_H_




//the signals that check to see if we are in limitations

/*! @brief Raises the alarms
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */
void SetRaise(void);


/*! @brief Writes into my array from ADC
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */
void ClearRaise(void);


/*! @brief Writes into my array from ADC
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */
void SetLower(void);


/*! @brief Writes into my array from ADC
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */
void ClearLower(void);

/*! @brief Writes into my array from ADC
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */
void SetHigher(void);

/*! @brief Writes into my array from ADC
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */
void ClearHigher(void);







#endif /* SOURCES_OUTPUT_H_ */
