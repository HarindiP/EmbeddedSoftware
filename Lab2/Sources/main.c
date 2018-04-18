/* ###################################################################
 **     Filename    : main.c
 **     Project     : Lab2
 **     Processor   : MK70FN1M0VMJ12
 **     Version     : Driver 01.01
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2015-07-20, 13:27, # CodeGen: 0
 **     Abstract    :
 **         Main module.
 **         This module contains user's application code.
 **     Settings    :
 **     Contents    :
 **         No public methods
 **
 ** ###################################################################*/
/*!
 ** @file main.c
 ** @version 2.0
 ** @brief
 **         Main module.
 **         This module contains user's application code.
 */
/*!
 **  @addtogroup main_module main module documentation
 **  @{
 */
/* MODULE main */


// CPU module - contains low level hardware initialization routines
#include "Cpu.h"
#include "Events.h"
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

//Sources module
#include "types.h"
#include "UART.h"
#include "packet.h"
#include "FIFO.h"
#include "SCP.h"
#include "LEDs.h"
#include "Flash.h"

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */
  //Tower Number Initialization
  towerNb.l = 5605;	//my student nb is 13115605
  //Tower Mode Initialization
  towerMd.l = 0;
  // Baud Rate and Module Clock
  uint32_t baudRate = 115200;
  uint32_t moduleClk = CPU_BUS_CLK_HZ;

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */

  // Initialization of communication
  if (Packet_Init(baudRate, moduleClk) && Flash_Init() && LEDs_Init())
    {
      //light on the orange LED
      LEDs_On(LED_ORANGE);
      //writting tower number and mode in flash
      Flash_AllocateVar(&NvTowerNb, sizeof(*NvTowerNb));
      Flash_Write16((uint16_t *)NvTowerNb, 5605);
      Flash_AllocateVar(&NvTowerMd, sizeof(*NvTowerMd));
      Flash_Write16((uint16_t *)NvTowerMd, 1);
      //sending start up values
      SendStartUpValues();
    }

  for (;;)	//Should we put that in the previous if loop ?
    {
      /*Checks the status of the serial port*/
      UART_Poll();

      /*If we have a packet, we can check Serial Protocol Commands */
      if(Packet_Get())
	{
	  if(!Packet_Acknowledgement_Required(Packet_Command))		/*Cases without Packet Acknowledgement required*/
	    {
	      switch (Packet_Command)
	      {
		case Get_Start_Up_Values :		//Command is : Special - Get Start up Values
		  SendStartUpValues();
		  break;
		case Get_Version :		//Command is : Special – Get version
		  SendVersion();
		  break;
		case Tower_Number :		//Command is : Tower number (get & set)
		  if(Packet_Parameter1 == 1)		// Command is : get Tower Nb
		    {
		      SendTowerNumber();
		    }
		  else if (Packet_Parameter2==2)	//Command is : set Tower Number
		    {
		      SetTowerNumber();
		    }
		  break;
		case Tower_Mode :		//Command is : Tower Mode (get & set)
		  if(Packet_Parameter1 == 1)		// Command is : get Tower Mode
		    {
		      SendTowerMode();
		    }
		  else if (Packet_Parameter2==2)	//Command is : set Tower Mode
		    {
		      SetTowerMode();
		    }
		  break;
		case Flash_Read_Byte :
		  ReadByte(Packet_Parameter1);
		  break;
		case Flash_Program_Byte :
		  ProgramByte(Packet_Parameter1,Packet_Parameter3);
		  break;
		default:	//Unknown command
		  //Do nothing or return command with NAK
		  //Packet_Put(0b1000000 | Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
		  break;
	      }
	    }
	  else
	    {
	      switch(PACKET_ACK_MASK|Packet_Command)	/*Cases with Packet Acknowledgement required*/
	      {
		case Get_Start_Up_Values :		//Command is : Special - Get Start up Values
		  if(SendStartUpValues())	//if Send Tower Number is ok,
		    {
		      Packet_ACK();		//ACK is sent
		    }
		  else
		    {
		      Packet_NAK();		//otherwise, NAK is sent
		    }
		  break;
		case Get_Version :		//Command is : Special – Get version
		  if(SendVersion())		//Send Tower version
		    {
		      Packet_ACK();
		    }
		  else
		    {
		      Packet_NAK();
		    }
		  break;
		case Tower_Number :		//Command is : Tower number (get & set)
		  if(Packet_Parameter1 == 1)		// Command is : get Tower Nb
		    {
		      if(SendTowerNumber())	//Send Tower Number
			{
			  Packet_ACK();
			}
		      else
			{
			  Packet_NAK();
			}
		    }
		  else if(Packet_Parameter1 == 2)	//Command is : set Tower Number
		    {
		      if(SetTowerNumber())	//Send Tower Number
			{
			  Packet_ACK();
			}
		      else
			{
			  Packet_NAK();
			}
		    }
		  break;
		case Tower_Mode :		//Command is : Tower Mode (get & set)
		  if(Packet_Parameter1 == 1)		// Command is : get Tower Mode
		    {
		      if(SendTowerMode())	//Send Tower Mode
			{
			  Packet_ACK();
			}
		      else
			{
			  Packet_NAK();
			}
		    }
		  else if(Packet_Parameter1 == 2)	//Command is : set Tower Mode
		    {
		      if(SetTowerMode())	//Send Tower Mode
			{
			  Packet_ACK();
			}
		      else
			{
			  Packet_NAK();
			}
		    }
		  break;
		case Flash_Program_Byte :		//Command is : Special - Get Start up Values
		  if(ProgramByte(Packet_Parameter1,Packet_Parameter3))	//if Send Tower Number is ok,
		    {
		      Packet_ACK();		//ACK is sent
		    }
		  else
		    {
		      Packet_NAK();		//otherwise, NAK is sent
		    }
		  break;
		case Flash_Read_Byte :		//Command is : Special – Get version
		  if(ReadByte(Packet_Parameter1))		//Send Tower version
		    {
		      Packet_ACK();
		    }
		  else
		    {
		      Packet_NAK();
		    }
		  break;
		default:	//Unknown command
		  //Do nothing or return command with NAK
		  //Packet_Put(0b1000000 | Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
		  break;
	      }
	    }
	}
    }

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.5 [05.21]
 **     for the Freescale Kinetis series of microcontrollers.
 **
 ** ###################################################################
 */
