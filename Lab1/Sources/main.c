/* ###################################################################
**     Filename    : main.c
**     Project     : Lab1
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
** @version 1.0
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

//If we dont put that here, we have an error "undeclared variable Packet_***, but does it raise issues ?
uint8_t 	Packet_Command,		/*!< The packet's command */
		Packet_Parameter1, 	/*!< The packet's 1st parameter */
		Packet_Parameter2, 	/*!< The packet's 2nd parameter */
		Packet_Parameter3,	/*!< The packet's 3rd parameter */
		Packet_Checksum;	/*!< The packet's checksum */

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */
  //Tower Number
  uint16union_t towerNb;	//my student nb is 13115605
  towerNb.s.Lo=5605%256;	// this representation makes things easier with the Communication Protocol
  towerNb.s.Hi=5605/256;



  // Baud Rate and Module Clock
  uint32_t baudRate = 38400;
  uint32_t moduleClk = CPU_BUS_CLK_HZ;


  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */

  // Initialization of communication
  Packet_Init(baudRate, moduleClk);
  Packet_Put(0x04,0,0,0);			//Send Tower Start up
  Packet_Put(0x09,'v',1,0);			//Send Tower Version V1.0
  Packet_Put(0x0B,1,towerNb.s.Lo,towerNb.s.Hi);	//Send Tower Number


  //Communication starts
  for (;;)
    {
      /*Checks the status of the serial port*/
      UART_Poll();

      /*If we have a packet, we can check Serial Protocol Commands */
      if(Packet_Get())
	{
	  switch (Packet_Command)
	  {
	    /*Cases without Packet Acknowledgement*/
	    case 0x04 :		//Command is : Special - Get Start up Values
	      Packet_Put(0x04,0,0,0);			//Send Tower Start up
	      Packet_Put(0x09,'v',1,0);			//Send Tower Version V1.0
	      Packet_Put(0x0B,1,towerNb.s.Lo,towerNb.s.Hi);	//Send Tower Number
	      break;
	    case 0x09 :		//Command is : Special – Get version
	      Packet_Put(0x09,'v',1,0);		//Send Tower version
	      break;
	    case 0x0B :		//Command is : Tower number (get & set)
	      if(Packet_Parameter1 == 1)		// get Tower Nb
		{
		  Packet_Put(0x0B,1,towerNb.s.Lo,towerNb.s.Hi);	//Send Tower Number
		}
	      else					//Paramter1=2, set Tower Number
		{
		  towerNb.s.Lo = Packet_Parameter2;	//LSB
		  towerNb.s.Hi = Packet_Parameter3;	//MSB
		  Packet_Put(0x0B,1,towerNb.s.Lo,towerNb.s.Hi);	//Send Tower Number
		}
	      break;

	      /*Cases with Packet Acknowledgement*/
	    case 0x84 :		//Command is : Special - Get Start up Values
	      if(Packet_Put(0x04,0,0,0) &&			//Send Tower Start up
	      Packet_Put(0x09,'v',1,0) &&			//Send Tower Version
	      Packet_Put(0x0B,1,towerNb.s.Lo,towerNb.s.Hi))	//Send Tower Number
		{
		  Packet_Put(Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
		}
	      else
		{
		  Packet_Put(0x04,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
		}
	      break;
	    case 0x89 :		//Command is : Special – Get version
	      if(Packet_Put(0x09,'v',1,0))		//Send Tower version
		{
		  Packet_Put(Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
		}
	      else
		{
		  Packet_Put(0x09,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
		}
	      break;
	    case 0x8B :		//Command is : Tower number (get & set)
	      if(Packet_Parameter1 == 1)		// get Tower Nb
		{
		  if(Packet_Put(0x0B,1,towerNb.s.Lo,towerNb.s.Hi))	//Send Tower Number
		    {
		      Packet_Put(Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
		    }
		  else
		    {
		      Packet_Put(0x0B,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
		    }
		}
	      else					//Paramter1=2, set Tower Number
		{
		  towerNb.s.Lo = Packet_Parameter2;	//LSB
		  towerNb.s.Hi = Packet_Parameter3;	//MSB
		  if(Packet_Put(0x0B,1,towerNb.s.Lo,towerNb.s.Hi))	//Send Tower Number
		    {
		      Packet_Put(Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
		    }
		  else
		    {
		      Packet_Put(0x0B,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
		    }

		}
	      break;

	    default:	//Unknown command
	      //Do nothing or return command with NAK
	      //Packet_Put(0b1000000 | Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
	      break;
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
