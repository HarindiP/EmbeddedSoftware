/*
 * LEDs.c
 *
 *  @brief Routines to access the LEDs on the TWR-K70F120M.
 *
 *  This contains the functions for operating the LEDs.
 *
 *  Created on: 6 Apr 2018
 *      Author: 13115605
 */




// new types
#include "types.h"

/*! @brief LED to pin mapping on the TWR-K70F120M
 *
 */
typedef enum
{
  LED_ORANGE = (1 << 11), //PTA11  59.2.6 59.3.2 -> mask on bit 11 here  59.3.3/4
  LED_YELLOW = (1 << 28),
  LED_GREEN = (1 << 29),
  LED_BLUE = (1 << 10)
} TLED;

/*! @brief Sets up the LEDs before first use.
 *
 *  @return bool - TRUE if the LEDs were successfully initialized.
 */
bool LEDs_Init(void)
{
  /*Activate PORTA*/
  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;

  /*Enabeling LEDs Pin*/
  PORTA_PCR11 |= PORT_PCR_MUX(3);		//see chapter 10
  PORTA_PCR28 |= PORT_PCR_MUX(3);
  PORTA_PCR29 |= PORT_PCR_MUX(3);
  PORTA_PCR10 |= PORT_PCR_MUX(3);
  GPIO_PDOR;

  return true;
}

/*! @brief Turns an LED on.
 *
 *  @param color The color of the LED to turn on.
 *  @note Assumes that LEDs_Init has been called.
 */
void LEDs_On(const TLED color)
{

}

/*! @brief Turns off an LED.
 *
 *  @param color THe color of the LED to turn off.
 *  @note Assumes that LEDs_Init has been called.
 */
void LEDs_Off(const TLED color)
{

}

/*! @brief Toggles an LED.
 *
 *  @param color THe color of the LED to toggle.
 *  @note Assumes that LEDs_Init has been called.
 */
void LEDs_Toggle(const TLED color)
{

}







