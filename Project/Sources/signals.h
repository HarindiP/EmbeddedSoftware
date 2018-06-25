/*
 * output.h
 *
 *  Created on: 13 Jun 2018
 *      Author: 12443166
 */

#ifndef SOURCES_SIGNALS_H_
#define SOURCES_SIGNALS_H_


//the signals that check to see if we are in limitations


/*! @brief sets the raise signal when voltage below limit
 *
 *  @param
 *  @note
 */
void SignalsSetHigher(void);


/*! @brief clear the raise signal when voltage back into limitation
 *
 *  @param
 *  @note
 */
void SignalsClearHigher(void);


/*! @brief Sets the lower signal when voltage above limit
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */
void SignalsSetLower(void);


/*! @brief clears the lower signal when voltage back into limitation
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */
void SignalsClearLower(void);


/*! @brief Sets the alarm signal when voltage out of bounds
 *
 *  @param
 *  @note
 */
void SignalsSetALarm(void);


/*! @brief Clears the alarm signal when voltage out of bounds
 *
 *  @param
 *  @note
 */
void SignalsClearAlarm(void);

/*! @brief Clears all the set Signals
 *
 *  @param
 *  @note
 */
void SignalsClearAll(void);



#endif /* SOURCES_SIGNALS_H_ */
