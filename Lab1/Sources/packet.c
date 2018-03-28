/*
 * packet.c
 *
 *  Created on: 28 Mar 2018
 *      Author: 13115605
 */


#include "packet.h"

/*! @brief Initializes the packets by calling the initialization routines of the supporting software modules.
 *
 *  @param baudRate The desired baud rate in bits/sec.
 *  @param moduleClk The module clock rate in Hz
 *  @return bool - TRUE if the packet module was successfully initialized.
 */
bool Packet_Init(const uint32_t baudRate, const uint32_t moduleClk){
    UART_Init(baudRate,moduleClk);
}

/*! @brief Attempts to get a packet from the received data.
 *
 *  @return bool - TRUE if a valid packet was received.
 */
bool Packet_Get(void){

}

/*! @brief Builds a packet and places it in the transmit FIFO buffer.
 *
 *  @return bool - TRUE if a valid packet was sent.
 */
bool Packet_Put(const uint8_t command, const uint8_t parameter1, const uint8_t parameter2, const uint8_t parameter3){

}


