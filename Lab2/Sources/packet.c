/*
 * packet.c
 *
 *  Created on: 6 Apr 2018
 *      Author: 13115605
 */

#include "packet.h"
#include "UART.h"
TPacket Packet;



// Acknowledgment bit mask
const uint8_t PACKET_ACK_MASK = 0b10000000;

/*! @brief Private Generating checksum function.
 *
 *  @return uint8_t - Packet CheckSum
 */
uint8_t CheckSum(const uint8_t command, const uint8_t parameter1, const uint8_t parameter2, const uint8_t parameter3)
{
  uint8_t checksum = Packet_Command^Packet_Parameter1^Packet_Parameter2^Packet_Parameter3;
  return checksum;
}

/*! @brief Private ACK checking function
 *
 *  @return bool - TRUE if acknowledgement required
 */
bool Packet_Acknowledgement_Required(const uint8_t command)
{
  return (command & PACKET_ACK_MASK);
}

/*! @brief Initializes the packets by calling the initialization routines of the supporting software modules.
 *
 *  @param baudRate The desired baud rate in bits/sec.
 *  @param moduleClk The module clock rate in Hz.
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
      if (count == 0)
	{
	  Packet_Command = tempData;
	  count++;
	}
      else if (count == 1)
	{
	  Packet_Parameter1 = tempData;
	  count++;
	}
      else if (count == 2)
	{
	  Packet_Parameter2 = tempData;
	  count++;
	}
      else if (count == 3)
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
	  Packet_Checksum = CheckSum(Packet_Command, Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);//Packet_Command^Packet_Parameter1^Packet_Parameter2^Packet_Parameter3;
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
bool Packet_Put(const uint8_t command, const uint8_t parameter1, const uint8_t parameter2, const uint8_t parameter3)
{
  /*if (UART_OutChar(command))
    {
      if (UART_OutChar(parameter1))
	{
	  if (UART_OutChar(parameter2))
	    {
	      if (UART_OutChar(parameter3))
		{
		  return (UART_OutChar(CheckSum(command, parameter1,parameter2,parameter3)));
		}
	    }
	}
    }
  return false;*/
  return UART_OutChar(command) & UART_OutChar(parameter1) & UART_OutChar(parameter2)& UART_OutChar(parameter3) & UART_OutChar(CheckSum(command, parameter1,parameter2,parameter3));
}

