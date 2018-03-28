/*
 * UART.c
 *
 *  Created on: 28 Mar 2018
 *      Author: 13115605
 */


#include "UART.h"

static TFIFO TxFIFO, RxFIFO;

/*! @brief Sets up the UART interface before first use.
 *
 *  @param baudRate The desired baud rate in bits/sec.
 *  @param moduleClk The module clock rate in Hz
 *  @return bool - TRUE if the UART was successfully initialized.
 */
bool UART_Init(const uint32_t baudRate, const uint32_t moduleClk){
    //Turn UART2 on
    SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;
    // Tx Pin
    PORTE_PCR16 = PORT_PCR_MUX(3);
    // Rx Pin
    PORTE_PCR17 = PORT_PCR_MUX(3);

    /*Activate PORTE*/
    SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;

    /*Baud settings*/
    uint16_t sbr = moduleClk/(16*baudRate);
    uint8_t brfd = (moduleClk-16*baudRate*sbr)*32/(16*baudRate);

    UART2_BDH




    /*// Clear bit 4 which is w1c
    00010011
    // Bug in line below !!
    REGISTER_WITH_W1C_BITS |= 0b00010000;
    // Correct :
    REGISTER_WITH_W1C_BITS = 0b00010000;
    00000011*/
}

/*! @brief Get a character from the receive FIFO if it is not empty.
 *
 *  @param dataPtr A pointer to memory to store the retrieved byte.
 *  @return bool - TRUE if the receive FIFO returned a character.
 *  @note Assumes that UART_Init has been called.
 */
bool UART_InChar(uint8_t * const dataPtr){
    FIFO_Get(RxFIFO,dataPtr);

}

/*! @brief Put a byte in the transmit FIFO if it is not full.
 *
 *  @param data The byte to be placed in the transmit FIFO.
 *  @return bool - TRUE if the data was placed in the transmit FIFO.
 *  @note Assumes that UART_Init has been called.
 */
bool UART_OutChar(const uint8_t data){
    FIFO_Put(TxFIFO,data);
}

/*! @brief Poll the UART status register to try and receive and/or transmit one character.
 *
 *  @return void
 *  @note Assumes that UART_Init has been called.
 */
void UART_Poll(void){

}

