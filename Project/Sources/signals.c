/*
 * output.c
 *
 *  Created on: 13 Jun 2018
 *      Author: 12443166
 */

/*Space to create the limits and check whether in bounds*/

#include "analog.h"
#include "UsefulFunctions.h"

#define BITS_PER_VOLT 3276.7

/*! @brief sets the raise signal when voltage below limit
 *
 *  @param
 *  @note
 */
void SignalsSetRaise(void)
{
  Analog_Put(1, VoltagetoAnalog(5));
}


/*! @brief clear the raise signal when voltage back into limitation
 *
 *  @param
 *  @note
 */
void SignalsClearRaise(void)
{
  Analog_Put(1, VoltagetoAnalog(0));
}


/*! @brief Sets the lower signal when voltage above limit
 *
 *  @param
 *  @note assume
 */
void SignalsSetLower(void)
{
  Analog_Put(2, VoltagetoAnalog(5));
}


/*! @brief clears the lower signal when voltage back into limitation
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */
void SignalsClearLower(void)
{
  Analog_Put(2, VoltagetoAnalog(0));
}


/*! @brief Sets the alarm signal when voltage out of bounds
 *
 *  @param
 *  @note
 */
void SignalsSetALarm(void)
{
  Analog_Put(3, VoltagetoAnalog(5));
}


/*! @brief Clears the alarm signal when voltage out of bounds
 *
 *  @param
 *  @note
 */
void SignalsClearAlarm(void)
{
  Analog_Put(3, VoltagetoAnalog(0));
}


