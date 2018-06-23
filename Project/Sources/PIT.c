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


OS_ECB* PITAccess;  //used for vrms every 1.25ms
OS_ECB* PIT1Access; //used for vrms every 5s

bool PIT_Init(const uint32_t moduleClk, void (*userFunction)(void*), void* userArguments)
{
//  EnterCritical(); // No need as interrupts should already be disabled whe this function is called

  /*Gets the period of the clock from Freq*/
  Clkperiod = 1e9 / moduleClk ;

  /*Enable clock gate to PIT module*/
  SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;

  /* Enable PIT*/
  PIT_MCR &= ~PIT_MCR_MDIS_MASK; //MDIS Because this bit enables or disables pit timers

  /*freezes while debugging*/
  PIT_MCR |= PIT_MCR_FRZ_MASK; // FRZ because this allows the timers to be stopped

  /*Enable interupts for PIT 0*/
  PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK;


  /*Enable interupts for PIT1*/
   PIT_TCTRL1 |= PIT_TCTRL_TIE_MASK;


   /*********************Initialise NVIC**************************/
  //NVICISER2 = (IQR%32)   IRQ%32  IPR=17 IRQ=68
  /*clears any pending requests*/
  NVICICPR2 = (1 << (68 % 32));

  /*Enable interupts from PIT Module*/
  NVICISER2 = (1 << (68 % 32));

  /***********************Initialise NVIC**************************/
  //NVICISER2 = (IQR%32)   IRQ%32  IPR=17 IRQ=68
  /*clears any pending requests*/
  NVICICPR2 = (1 << (69 % 32));

  /*Enable interupts from PIT Module*/
  NVICISER2 = (1 << (69 % 32));

  //Create Semaphore for PIT0
  PITAccess = OS_SemaphoreCreate(0);


  //Create Semaphore for PIT1
  PIT1Access = OS_SemaphoreCreate(0);

  /*Enable timer*/
//  PIT_Enable(true);
//
//  /*Enable timer*/
//  PIT1_Enable(true);


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
      PIT_LDVAL0 = (period / Clkperiod) -1 ;
    }

  PIT_Enable(true);
  PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK;



}

void PIT1_Set(const uint64_t period, const bool restart)
{
  //  (LDVAL trigger = (period / clock period) -1)
  // clock period = 1/freq

  if (restart)
    {
      PIT1_Enable(false);
      PIT_LDVAL1 = ((period * 1000000) / Clkperiod) -1 ;
      PIT1_Enable(true);
      PIT_TCTRL1 |= PIT_TCTRL_TIE_MASK;
    }

  else
  {
    PIT_LDVAL1 = ((period * 1000000) / Clkperiod) -1 ;
    PIT1_Enable(true);
    PIT_TCTRL1 |= PIT_TCTRL_TIE_MASK;
  }



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

void PIT1_Enable(const bool enable)
{
  if (enable)
    {
      (PIT_TCTRL1 |= PIT_TCTRL_TEN_MASK);
    }
  else
    {
      (PIT_TCTRL1 &= ~PIT_TCTRL_TEN_MASK); //disable
    }
}


void __attribute__ ((interrupt)) PIT_ISR(void)
{
  OS_ISREnter();

  if (PIT_TFLG0 & PIT_TFLG_TIF_MASK) // Check if timeout has occurred
  {
    PIT_TFLG0 |= PIT_TFLG_TIF_MASK; // Clear timer interrupt flag
    OS_SemaphoreSignal(PITAccess); // Signal PIT thread to tell it can run
  }

  OS_ISRExit(); // End of servicing interrupt
}


void __attribute__ ((interrupt)) PIT1_ISR(void)
{
  OS_ISREnter();

  if (PIT_TFLG1 & PIT_TFLG_TIF_MASK) // Check if timeout has occurred
  {
    PIT_TFLG1 |= PIT_TFLG_TIF_MASK; // Clear timer interrupt flag
    OS_SemaphoreSignal(PIT1Access); // Signals PIT thread
  }

  OS_ISRExit(); // End of servicing interrupt
}



/*!
 ** @}
 */
