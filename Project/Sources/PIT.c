/*! @file PIT.c
 *
 *  @brief Routines for controlling Periodic Interrupt Timer (PIT) on the TWR-K70F120M.
 *
 *  This contains the functions for operating the periodic interrupt timer (PIT).
 *
 *  @author Harindi&Coralie
 *  @date 2015-08-22
 */
/*!
 **  @addtogroup PIT_module PIT module documentation
 **  @{
 */
/* MODULE PIT */

// new types
#include "types.h"
#include "PIT.h"
#include "MK70F12.h"
#include "PE_Types.h"
#include "LEDs.h"


static uint32_t Clkperiod; //ask coralie if it should be static
const static uint32_t PITPeriod = 5e+8; /*500ms to nanoseconds*/	// MOVE TO MAIN!!!!
static void (*UserFunction)(void*);
static void* UserArguments;

OS_ECB* PITAccess;

bool PIT_Init(const uint32_t moduleClk, void (*userFunction)(void*), void* userArguments)
{
//  EnterCritical(); // No need as interrupts should already be disabled whe this function is called
  /*aSK CORALIE IF ALL SHOULD BE IN IF STATEMENTS USED TEST*/
  /*Gets the period of the clock from freq*/
  Clkperiod = 1e9 / moduleClk ;

  UserFunction = userFunction;
  UserArguments = userArguments;

  /*Enable clock gate to PIT module*/
  SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;

  /* Enable PIT*/
  PIT_MCR &= ~PIT_MCR_MDIS_MASK; //MDIS Because this bit enables or disables pit timers

  /*freezes while debugging*/
  PIT_MCR |= PIT_MCR_FRZ_MASK; // FRZ because this allows the timers to be stopped

  /*Enable interupts*/
  PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK; //Are these regs connected and how?

  /*Initialise NVIC*/
  //NVICISER2 = (IQR%32)   IRQ%32  IPR=17 IRQ=68
  /*clears any pending requests*/
  NVICICPR2 = (1 << (68 % 32));

  /*Enable interupts from PIT Module*/
  NVICISER2 = (1 << (68 % 32));

  //Create Semaphore
  PITAccess = OS_SemaphoreCreate(0);

  /*Enable timer*/
  PIT_Enable(true);

//  /*Sets timer*/
  PIT_Set(PITPeriod,true);

//  ExitCritical();

  return true;
}


void PIT_Set(const uint32_t period, const bool restart)
{
  //  (LDVAL trigger = (period / clock period) -1)
  // clock period = 1/freq

  if (restart)
    {
      PIT_Enable(false);
    }

  PIT_LDVAL0 = (period / Clkperiod) -1 ;

  PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK;
  PIT_Enable(true);


}


void PIT_Enable(const bool enable)
{
  if (enable)
    {
      (PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK);
    }
  else
    {
      (PIT_TCTRL0 &= ~PIT_TCTRL_TEN_MASK); //disable
    }
}


void __attribute__ ((interrupt)) PIT_ISR(void)
{
  OS_ISREnter();
  /*Clear interupt flag*/
  PIT_TFLG0 |= PIT_TFLG_TIF_MASK;

//  if (UserFunction)
//  (*UserFunction)(UserArguments);

  OS_SemaphoreSignal(PITAccess);
  OS_ISRExit();
}
//CALL USER FUCTION PAGE 4/7 LAB3

/*!
 ** @}
 */
