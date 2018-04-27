/*
 * PIT.c
 *
 *  Created on: 27 Apr 2018
 *      Author: Harindi Perera
 *      chapter 44
 */

// new types
#include "PIT.h"

/*! @brief Sets up the PIT before first use.
 *
 *  Enables the PIT and freezes the timer when debugging.
 *  @param moduleClk The module clock rate in Hz.
 *  @param userFunction is a pointer to a user callback function.
 *  @param userArguments is a pointer to the user arguments to use with the user callback function.
 *  @return bool - TRUE if the PIT was successfully initialized.
 *  @note Assumes that moduleClk has a period which can be expressed as an integral number of nanoseconds.
 */
bool PIT_Init(const uint32_t moduleClk, void (*userFunction)(void*), void* userArguments)
{

  //CPU
  //configure the right regs
  //moduleClk = CPU_BUS_CLK_HZ;

  // ENable PIT
  PIT_MCR &= ~PIT_MCR_MDIS_MASK; //MDIS Because this bit enables or disables pit timers

  //freezes while debugging
  PIT_MCR = PIT_MCR_FRZ_MASK; // FRZ because this allows the timers to be stopped

  //Enable interupts
  PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK; //Are these regs connected and how?

  // NVIC

  //NVICISER2 = (IQR%32)   IRQ%32  IPR=17 IRQ=70
  //NVICISER2[17] = //

  return true;
}

/*! @brief Sets the value of the desired period of the PIT.
 *
 *  @param period The desired value of the timer period in nanoseconds.
 *  @param restart TRUE if the PIT is disabled, a new value set, and then enabled.
 *                 FALSE if the PIT will use the new value after a trigger event.
 *  @note The function will enable the timer and interrupts for the PIT.
 */
void PIT_Set(const uint32_t period, const bool restart)
{



}

/*! @brief Enables or disables the PIT.
 *
 *  @param enable - TRUE if the PIT is to be enabled, FALSE if the PIT is to be disabled.
 */
void PIT_Enable(const bool enable)
{
  if (enable)
    {
      (PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK);
    }
  return (PIT_TCTRL0 &= ~PIT_TCTRL_TEN_MASK); //disable
}

/*! @brief Interrupt service routine for the PIT.
 *
 *  The periodic interrupt timer has timed out.
 *  The user callback function will be called.
 *  @note Assumes the PIT has been initialized.
 */
void __attribute__ ((interrupt)) PIT_ISR(void);
//CALL USER FUCTION PAGE 4/7 LAB3


