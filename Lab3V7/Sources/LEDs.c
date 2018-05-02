/*! @file LEDs.c
 *
 *  @brief Routines to access the LEDs on the TWR-K70F120M.
 *
 *  This contains the functions for operating the LEDs.
 *
 *  @author Coralie&Harindi
 *  @date 2018-04-06
 */
/*!
 **  @addtogroup LEDs_module LEDs module documentation
 **  @{
 */
/* MODULE LEDs */


// new types
#include "types.h"
#include "Cpu.h"
#include "PE_Types.h"

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
 // EnterCritical();
  /*Activate PORTA*/
  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;

  /*Enabeling LEDs Pin*/
  PORTA_PCR11 |= PORT_PCR_MUX(1);		//see chapter 10
  PORTA_PCR28 |= PORT_PCR_MUX(1);
  PORTA_PCR29 |= PORT_PCR_MUX(1);
  PORTA_PCR10 |= PORT_PCR_MUX(1);

  /*Setting  the right pins as OutPut */
  GPIOA_PDDR = LED_ORANGE | LED_YELLOW | LED_GREEN | LED_BLUE;

  GPIOA_PSOR = LED_ORANGE | LED_YELLOW | LED_GREEN | LED_BLUE;

  //ExitCritical();
  return true;
}

/*! @brief Turns an LED on.
 *
 *  @param color The color of the LED to turn on.
 *  @note Assumes that LEDs_Init has been called.
 */


void LEDs_On(const TLED color)
{
  GPIOA_PCOR = color;
//  switch (color){
//      case LED_BLUE :
//	GPIOA_PCOR |= GPIO_PCOR_PTCO(10);
//        break;
//      case LED_GREEN :
//	GPIOA_PCOR |= GPIO_PCOR_PTCO(29);
//        break;
//      case LED_YELLOW :
//	GPIOA_PCOR |= GPIO_PCOR_PTCO(28);
//        break;
//      case LED_ORANGE :
//	GPIOA_PCOR |= GPIO_PCOR_PTCO(11);
//        break;
//    }

}

/*! @brief Turns off an LED.
 *
 *  @param color THe color of the LED to turn off.
 *  @note Assumes that LEDs_Init has been called.
 */
void LEDs_Off(const TLED color)
{
  GPIOA_PSOR = color;
//  switch (color){
//      case LED_BLUE :
//        GPIOA_PSOR |= GPIO_PCOR_PTCO(10);
//        break;
//      case LED_GREEN :
//        GPIOA_PSOR |= GPIO_PCOR_PTCO(29);
//        break;
//      case LED_YELLOW :
//        GPIOA_PSOR |= GPIO_PCOR_PTCO(28);
//        break;
//      case LED_ORANGE :
//        GPIOA_PSOR |= GPIO_PCOR_PTCO(11);
//        break;
//    }
}

/*! @brief Toggles an LED.
 *
 *  @param color THe color of the LED to toggle.
 *  @note Assumes that LEDs_Init has been called.
 */
void LEDs_Toggle(const TLED color)
{
  GPIOA_PTOR = color;
}


/*!
 ** @}
 */

