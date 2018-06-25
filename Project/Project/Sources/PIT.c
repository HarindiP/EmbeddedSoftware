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
#include "Regulation.h"
#include "Requirements.h"


static uint32_t Clkperiod; //ask coralie if it should be static
const static uint32_t PITPeriod = 5e+8; /*500ms to nanoseconds*/	// MOVE TO MAIN!!!!
static void (*UserFunction)(void*);
static void* UserArguments;

OS_ECB* PIT0Access;
OS_ECB* PIT1Access;

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

  //PIT0
  /*Enable interupts*/
  PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK;
  /*Initialise NVIC*/
  //NVICISER2 = (IQR%32)   IRQ%32  IPR=17 IRQ=68
  /*clears any pending requests*/
  NVICICPR2 = (1 << (68 % 32));
  /*Enable interupts from PIT Module*/
  NVICISER2 = (1 << (68 % 32));

  //PIT1
  /*Enable interupts*/
  PIT_TCTRL1 |= PIT_TCTRL_TIE_MASK;
  /*Initialise NVIC*/
  //NVICISER2 = (IQR%32)   IRQ%32  IPR=17 IRQ=69
  /*clears any pending requests*/
  NVICICPR2 = (1 << (69 % 32));
  /*Enable interupts from PIT Module*/
  NVICISER2 = (1 << (69 % 32));

  //Create Semaphore
  PIT0Access = OS_SemaphoreCreate(0);
  PIT1Access = OS_SemaphoreCreate(0);

//  /*Enable timer*/
//  PIT0_Enable(true);
//  PIT1_Enable(true);
//  /*Sets timer*/
//  PIT0_Set(PITPeriod,true);
//  PIT1_Set(PITPeriod,true);

//  ExitCritical();

  return true;
}


void PIT0_Set(const uint32_t period, const bool restart)
{
  //  (LDVAL trigger = (period / clock period) -1)
  // clock period = 1/freq
  if (restart)
  {
    PIT0_Enable(false);
  }
  PIT_LDVAL0 = (period / Clkperiod) - 1 ;
  PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK;
  PIT0_Enable(true);
}

void PIT1_Set(const uint32_t period, const bool restart)
{
  if (restart)
  {
    PIT1_Enable(false);
  }

  PIT_LDVAL1 = (period / Clkperiod) * 1000000 - 1 ;
//  PIT_LDVAL1 = (period / Clkperiod) -1 ;
  PIT_TCTRL1 |= PIT_TCTRL_TIE_MASK;
  PIT1_Enable(true);
}


void PIT0_Enable(const bool enable)
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


void __attribute__ ((interrupt)) PIT0_ISR(void)
{
  OS_ISREnter();
  /*Clear interupt flag*/
  PIT_TFLG0 |= PIT_TFLG_TIF_MASK;

//  if (UserFunction)
//  (*UserFunction)(UserArguments);

  static int nbSampleTaken = 0;

  //Take a sample on each channel
//  int16_t sample;
//  Analog_Get(0,&sample);
//  TakeSample(Regulation_FullSampleA,sample);
//  Analog_Get(1,&sample);
//  TakeSample(Regulation_FullSampleB,sample);
//  Analog_Get(2,&sample);
//  TakeSample(Regulation_FullSampleC,sample);

  Analog_Get(0,Regulation_FullSampleA + nbSampleTaken);
  Analog_Get(1,Regulation_FullSampleB + nbSampleTaken);
  Analog_Get(2,Regulation_FullSampleC + nbSampleTaken);

  nbSampleTaken++;

  if(nbSampleTaken == NB_OF_SAMPLE)
  {
    nbSampleTaken = 0;
    //Stop taking samples
    PIT0_Enable(false);
    //Signal the treatment thread
    OS_SemaphoreSignal(FullSampleTaken);
  }
//  OS_SemaphoreSignal(PIT0Access);
  OS_ISRExit();
}

//void PIT0Thread(void* pData)
//{
//  for(;;)
//  {
//    OS_SemaphoreWait(PIT0Access,0);
////    //Toggle Green LED
////    LEDs_Toggle(LED_GREEN);
//
//    //    OS_SemaphoreSignal(SampleTaken);  //PIT is too quick, cant do that here
//  }
//}

void __attribute__ ((interrupt)) PIT1_ISR(void)
{
  OS_ISREnter();
  /*Clear interupt flag*/
  PIT_TFLG1 |= PIT_TFLG_TIF_MASK;

//  if (UserFunction)
//  (*UserFunction)(UserArguments);

  //Turn off Green LED
  LEDs_Off(LED_GREEN);
  Regulation_AlarmReached = true;
  PIT1_Enable(false);
//  LEDs_Toggle(LED_GREEN);

//  OS_SemaphoreSignal(PIT1Access);
  OS_ISRExit();
}

//void PIT1Thread(void* pData)
//{
//  for(;;)
//  {
//    OS_SemaphoreWait(PIT1Access,0);
//    //Signal a variable
//    Regulation_AlarmSet = true;
//  }
//}


/*!
 ** @}
 */
