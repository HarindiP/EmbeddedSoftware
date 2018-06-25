/*! @file PIT.h
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

#ifndef PIT_H
#define PIT_H

// new types
#include "types.h"
#include "MK70F12.h"
#include "OS.h"

//Semaphore to acces the Thread
OS_ECB* PIT0Access;
OS_ECB* PIT1Access;

/*! @brief Sets up the PIT before first use.
 *
 *  Enables the PIT and freezes the timer when debugging.
 *  @param moduleClk The module clock rate in Hz.
 *  @param userFunction is a pointer to a user callback function.
 *  @param userArguments is a pointer to the user arguments to use with the user callback function.
 *  @return bool - TRUE if the PIT was successfully initialized.
 *  @note Assumes that moduleClk has a period which can be expressed as an integral number of nanoseconds.
 */
bool PIT_Init(const uint32_t moduleClk, void (*userFunction)(void*), void* userArguments);

/*! @brief Sets the value of the desired period of the PIT.
 *
 *  @param period The desired value of the timer period in nanoseconds.
 *  @param restart TRUE if the PIT is disabled, a new value set, and then enabled.
 *                 FALSE if the PIT will use the new value after a trigger event.
 *  @note The function will enable the timer and interrupts for the PIT.
 */
void PIT0_Set(const uint32_t period, const bool restart);

/*! @brief Sets the value of the desired period of the PIT.
 *
 *  @param period The desired value of the timer period in nanoseconds.
 *  @param restart TRUE if the PIT is disabled, a new value set, and then enabled.
 *                 FALSE if the PIT will use the new value after a trigger event.
 *  @note The function will enable the timer and interrupts for the PIT.
 */
void PIT_Set(uint8_t index, const uint32_t period, const bool restart);

/*! @brief Enables or disables the PIT.
 *
 *  @param enable - TRUE if the PIT is to be enabled, FALSE if the PIT is to be disabled.
 */
void PIT0_Enable(const bool enable);

/*! @brief Enables or disables the PIT.
 *
 *  @param enable - TRUE if the PIT is to be enabled, FALSE if the PIT is to be disabled.
 */
void PIT_Enable(uint8_t index, const bool enable);


/*! @brief Interrupt service routine for the PIT0. Takes a sample on each channel of the VRR.
 *
 *  The periodic interrupt timer has timed out.
 *  The user callback function will be called.
 *  @note Assumes the PIT has been initialized.
 */
void __attribute__ ((interrupt)) PIT0_ISR(void);

///*! @brief
// *
// *  @param pData is not used but is required by the OS to create a thread.
// */
//void PIT0Thread(void* pData);

/*! @brief Interrupt service routine for the PIT1.
 *
 *  The periodic interrupt timer has timed out.
 *  The user callback function will be called.
 *  @note Assumes the PIT has been initialized.
 */
void __attribute__ ((interrupt)) PIT1_ISR(void);

/*! @brief Interrupt service routine for the PIT2.
 *
 *  The periodic interrupt timer has timed out.
 *  The user callback function will be called.
 *  @note Assumes the PIT has been initialized.
 */
void __attribute__ ((interrupt)) PIT2_ISR(void);

/*! @brief Interrupt service routine for the PIT3.
 *
 *  The periodic interrupt timer has timed out.
 *  The user callback function will be called.
 *  @note Assumes the PIT has been initialized.
 */
void __attribute__ ((interrupt)) PIT3_ISR(void);

///*! @brief toggle green LED
// *
// *  @param pData is not used but is required by the OS to create a thread.
// */
//void PIT1Thread(void* pData);

#endif

/*!
 ** @}
 */
