/*! @file
 *
 *  @brief I/O routines for UART communications on the TWR-K70F120M.
 *
 *  This contains the functions for operating the UART (serial port).
 *
 *  @author PMcL
 *  @date 2015-07-23
 */
/*!
 **  @addtogroup UART_module UART module documentation
 **  @{
 */

#ifndef UART_H
#define UART_H

// new types
#include "types.h"
#include "OS.h"


/*! @brief Sets up the UART interface before first use.
 *
 *  @param baudRate The desired baud rate in bits/sec.
 *  @param moduleClk The module clock rate in Hz.
 *  @return bool - TRUE if the UART was successfully initialized.
 */
bool UART_Init(const uint32_t baudRate, const uint32_t moduleClk);
 
/*! @brief Get a character from the receive FIFO if it is not empty.
 *
 *  @param dataPtr A pointer to memory to store the retrieved byte.
 *  @return bool - TRUE if the receive FIFO returned a character.
 *  @note Assumes that UART_Init has been called.
 */
bool UART_InChar(uint8_t * const dataPtr);
 
/*! @brief Put a byte in the transmit FIFO if it is not full.
 *
 *  @param data The byte to be placed in the transmit FIFO.
 *  @return bool - TRUE if the data was placed in the transmit FIFO.
 *  @note Assumes that UART_Init has been called.
 */
bool UART_OutChar(const uint8_t data);

/*! @brief Poll the UART status register to try and receive and/or transmit one character.
 *
 *  @return void
 *  @note Assumes that UART_Init has been called.
 */
void UART_Poll(void);

/*! @brief Put the byte to transmit from TxFIFO to UART_D reg
 *
 *  @param pData is not used but is required by the OS to create a thread.
 */
void UART_TxThread(void* pData);

/*! @brief Put the received byte in the RxFIFO
 *
 *  @param pData is not used but is required by the OS to create a thread.
 */
void UART_RxThread(void* pData);

/*! @brief Interrupt service routine for the UART.
 *
 *  @note Assumes the UART has been initialized.
 */
void __attribute__ ((interrupt)) UART_ISR(void);

#endif

/*!
 ** @}
 */
