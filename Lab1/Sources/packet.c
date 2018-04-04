/*
 * packet.c
 *
 *  Created on: 28 Mar 2018
 *      Author: 13115605
 */


#include "packet.h"
#include "UART.h"

// Packet structure
extern uint8_t 	Packet_Command,		/*!< The packet's command */
		Packet_Parameter1, 	/*!< The packet's 1st parameter */
		Packet_Parameter2, 	/*!< The packet's 2nd parameter */
		Packet_Parameter3,	/*!< The packet's 3rd parameter */
		Packet_Checksum;	/*!< The packet's checksum */

// Private Gen checksum function

/*! @brief Initializes the packets by calling the initialization routines of the supporting software modules.
 *
 *  @param baudRate The desired baud rate in bits/sec.
 *  @param moduleClk The module clock rate in Hz
 *  @return bool - TRUE if the packet module was successfully initialized.
 */
bool Packet_Init(const uint32_t baudRate, const uint32_t moduleClk)
{
    return UART_Init(baudRate,moduleClk);
}

/*! @brief Attempts to get a packet from the received data.
 *
 *  @return bool - TRUE if a valid packet was received.
 */
bool Packet_Get(void)
{
  uint8_t tempData;		//the data which will be put in the right parameter in the packet
   static uint8_t count = 0;	// the factor that decides which parameter the data goes into.

   while (UART_InChar(&tempData) && count<5)
   {
       if (count = 0)
 	{
 	  Packet_Command = tempData;
 	  count++;
 	}
       else if (count = 1)
 	{
 	  Packet_Parameter1 = tempData;
 	  count++;
 	}
 	else if (count = 2)
 	{
 	  Packet_Parameter2 = tempData;
 	  count++;
 	}
 	else if (count = 3)
 	{
 	  Packet_Parameter3 = tempData;
 	  count++;
 	}

       /* Finally when Ive got 4 bytes, the 5th byte I receive could be the checksum!!!!!!!!
        * So, I need to check the first 4 bytes, meaning I will generate a check sum, compare it to the 5th byte.
        * If it matches, YESSSSSSSSS -> return true
        * otherwise, return false*/

       else if (count == 4)
	{
	   Packet_Checksum = Packet_Command^Packet_Parameter1^Packet_Parameter2^Packet_Parameter3;
	   if(Packet_Checksum == tempData)
	   {
	       count = 0;
	       return true;
	   }
	   else
	   {
	       return false;
	   }
       }
   }
   return false;
}

/*! @brief Builds a packet and places it in the transmit FIFO buffer.
 *
 *  @return bool - TRUE if a valid packet was sent.
 */
bool Packet_Put(const uint8_t command, const uint8_t parameter1, const uint8_t parameter2, const uint8_t parameter3){
    // Stop if one of these does not work!
  bool b = UART_OutChar(command) && UART_OutChar(parameter1) && UART_OutChar(parameter2) && UART_OutChar(parameter3);
  return (b && UART_OutChar(command^parameter1^parameter2^parameter3));			//finally, calculate and put the checkSum
}


