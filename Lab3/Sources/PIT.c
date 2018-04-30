/*
 * PIT.c
 *
 *  Created on: 27 Apr 2018
 *      Author: Harindi Perera
 *      chapter 44
 */

// new types
#include "types.h"
#include "PIT.h"

static uint32_t Clkperiod; //ask coralie if it should be static
const static uint32_t PITPeriod


bool PIT_Init(const uint32_t moduleClk, void (*userFunction)(void*), void* userArguments)
{

  /*Gets the period of the clock from freq*/
  uint32_t Clkperiod = 1e9 / moduleClk ;

  /*Enable clock gate to PIT module*/
  SIMSCGC6 |= SIM_SCGC6_PIT_MASK;

  /* Enable PIT*/
  PIT_MCR &= ~PIT_MCR_MDIS_MASK; //MDIS Because this bit enables or disables pit timers

  /*freezes while debugging*/
  PIT_MCR = PIT_MCR_FRZ_MASK; // FRZ because this allows the timers to be stopped

  /*Enable interupts*/
  PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK; //Are these regs connected and how?

  /*Initialise NVIC*/
  //NVICISER2 = (IQR%32)   IRQ%32  IPR=17 IRQ=68

  /*clears any pending requests*/
  NVICICPR2 = (1 << (68 % 32));

  /*Enable interupts from PIT Module*/
  NVICISER2 = (1 << (68 % 32));

  /*Enable timer*/
  PIT_Enable(true);

  /*Sets timer*/
  PIT_Set(,true);


  return true;
}


void PIT_Set(const uint32_t period, const bool restart)
{
  //  (LDVAL trigger = (period / clock period) -1)
  // clock period = 1/freq
  PIT_LDVAL0 = (period / Clkperiod) -1 ;

  if (restart)
    {
      PIT_Enable(False);
    }
  else PIT_Enable(True);


}


void PIT_Enable(const bool enable)
{
  if (enable)
    {
      (PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK);
    }
  (PIT_TCTRL0 &= ~PIT_TCTRL_TEN_MASK); //disable
}



void __attribute__ ((interrupt)) PIT_ISR(void);
//CALL USER FUCTION PAGE 4/7 LAB3


