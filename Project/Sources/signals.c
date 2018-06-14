/*
 * output.c
 *
 *  Created on: 13 Jun 2018
 *      Author: 12443166
 */

/*Space to create the limits and check whether in bounds*/

/*! @brief sets the raise signal when voltage below limit
 *
 *  @param
 *  @note
 */
void SignalsSetRaise(void);


/*! @brief clear the raise signal when voltage back into limitation
 *
 *  @param
 *  @note
 */
void SignalsClearRaise(void);


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


