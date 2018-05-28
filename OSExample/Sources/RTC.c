/*! @file RTC.c
 *
 *  @brief Routines for controlling the Real Time Clock (RTC) on the TWR-K70F120M.
 *
 *  This contains the functions for operating the real time clock (RTC).
 *
 *  @author Coralie&Harindi
 *  @date 2018-04-27
 */
/*!
 **  @addtogroup RTC_module RTC module documentation
 **  @{
 */
/* MODULE RTC */

#ifndef RTC_H
#define RTC_H

// new types
#include "types.h"
#include "RTC.h"
#include "MK70F12.h"
#include "PE_Types.h"
#include "LEDs.h"
#include "OS.h"
#include "SCP.h"

static void (*UserFunction)(void*);
static void* UserArguments;

OS_ECB* RTCAccess;

/*! @brief Initializes the RTC before first use.
 *
 *  Sets up the control register for the RTC and locks it.
 *  Enables the RTC and sets an interrupt every second.
 *  @param userFunction is a pointer to a user callback function.
 *  @param userArguments is a pointer to the user arguments to use with the user callback function.
 *  @return bool - TRUE if the RTC was successfully initialized.
 */
bool RTC_Init(void (*userFunction)(void*), void* userArguments)
{
//  EnterCritical();
 //These arguments used for interupts
  UserFunction = userFunction;
  UserArguments = userArguments;

 /*Enable RTC clock module*/
  SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;

  /*check if tif flag is set, if it is disable counter and write time set when flag was set pg1384*/
  if (RTC_SR & RTC_SR_TIF_MASK)
    {
      /*Disable the time counter*/
      RTC_SR &= ~RTC_SR_TCE_MASK;
      /*Writes the time the tif flag was set pg1389 */
      RTC_TSR = RTC_TTSR;
    }

  /*Sets up the control register*/
  /* Oscillator 2pF Load Configuration: 1 enabled*/
  RTC_CR |= RTC_CR_SC2P_MASK;
//  /*Oscillator 4pF Load Configuration: 0 disabled*/
//  RTC_CR &= ~RTC_CR_SC4P_MASK;
//  /*Oscillator 8pF Load Configuration: 0 disabled*/
//  RTC_CR &= ~RTC_CR_SC8P_MASK;
  /*Oscillator 16pF Load Configuration: 1 enabled*/
  RTC_CR |= RTC_CR_SC16P_MASK;
  /*Once Oscillator Configuration are set enable the occilater to 32.something value*/
  RTC_CR |= RTC_CR_OSCE_MASK;
  /*Enable interupts for every second*/
  RTC_IER |= RTC_IER_TSIE_MASK; /*Time Seconds Interrupt Enable*/

  /*Disable the others but no sure if needed maybe good when testing*/
  RTC_IER &= ~RTC_IER_TIIE_MASK;
  RTC_IER &= ~RTC_IER_TOIE_MASK;
  RTC_IER &= ~RTC_IER_TAIE_MASK;

  /*Locks the control reg to ignore other writes*/
  RTC_LR &= ~RTC_LR_CRL_MASK;

  /*Wait an arbitrary amount of time*/
  for(int i = 0 ; i < 100000 ; i++);

  /*Enable timer counter*/
  RTC_SR |= RTC_SR_TCE_MASK;

  /*clear pending interupts from RTC module */
  NVICICPR2 = (1 << 3);
  /*Enable interups*/
  NVICISER2 = (1 << 3); /*the value 3 comes from the manual IQR%32 = 67%32*/

//  ExitCritical();
  RTCAccess = OS_SemaphoreCreate(0);
  return true;

}

/*! @brief Sets the value of the real time clock.
 *
 *  @param hours The desired value of the real time clock hours (0-23).
 *  @param minutes The desired value of the real time clock minutes (0-59).
 *  @param seconds The desired value of the real time clock seconds (0-59).
 *  @note Assumes that the RTC module has been initialized and all input parameters are in range.
 */
void RTC_Set(const uint8_t hours, const uint8_t minutes, const uint8_t seconds)
{
  const int secondsInhour = 60 * 60;
  const int secondsInDay = secondsInhour * 24;

  /*disable seconds timer*/
  RTC_SR &= ~RTC_SR_TCE_MASK;

  RTC_SR = (uint16_t)seconds +(uint16_t)minutes*60 +(uint16_t)hours*3600;

  /*enable seconds timer*/
  RTC_SR |= RTC_SR_TCE_MASK;
}

/*! @brief Gets the value of the real time clock.
 *
 *  @param hours The address of a variable to store the real time clock hours.
 *  @param minutes The address of a variable to store the real time clock minutes.
 *  @param seconds The address of a variable to store the real time clock seconds.
 *  @note Assumes that the RTC module has been initialized.
 */
void RTC_Get(uint8_t* const hours, uint8_t* const minutes, uint8_t* const seconds)
{
  //to read a vaild result make sure to have 2 consecutive identical reads
  int read1,read2;
  do
    {
      read1 = RTC_TSR;
      read2 = RTC_TSR;
    } while (read1 != read2);
  int rtcSeconds = read1;

  const int secondsInhour = 60 * 60;
  const int secondsInDay = secondsInhour * 24;

  //THE math that will convert
  int daySeconds = rtcSeconds % secondsInDay;
  *hours = daySeconds / secondsInhour;
  *minutes = (daySeconds % secondsInhour) / 60;
  *seconds = (daySeconds % secondsInhour) % 60;

}

void RTCThread(void* pData)
{
  for(;;)
  {
    OS_SemaphoreWait(RTCAccess,0);
    //Send Time
//    SendTime();
  }
}

/*! @brief Interrupt service routine for the RTC.
 *
 *  The RTC has incremented one second.
 *  The user callback function will be called.
 *  @note Assumes the RTC has been initialized.
 */
void __attribute__ ((interrupt)) RTC_ISR(void)
{
//  if (UserFunction)
//  (*UserFunction)(UserArguments);
  OS_ISREnter();
  //Toggle Yellow LED
  LEDs_Toggle(LED_YELLOW);
  OS_SemaphoreSignal(RTCAccess);
  OS_ISRExit();
}

#endif

/*!
 ** @}
 */
