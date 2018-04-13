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

//Communication commands : PC to Tower
#define Get_Start_Up_Values 0x04	//Command is : Special - Get Start up Values
#define Get_Version 0x09		//Command is : Special – Get version
#define Tower_Number 0x0B		//Command is : Tower number (get & set)
#define Tower_Mode 0x0D			//Command is : Tower Mode (get & set)
#define Flash_Read_Byte 0x08		//Command is : Flash - Read byte
#define Flash_Program_Byte 0x07		//Command is : Flash - Program byte


/*Tower Number*/
uint16union_t towerNb;
/*Tower Mode*/
uint16union_t towerMd;

/*Communication functions : */
bool SendStartUpValues()
{
  return (Packet_Put(0x04,0,0,0)			//Send Tower Start up
      &&Packet_Put(0x09,'v',1,0)			//Send Tower Version V1.0
      &&Packet_Put(0x0B,1,towerNb.s.Lo,towerNb.s.Hi)	//Send Tower Number
      /*&&Packet_Put(0x0D,1,towerMd.s.Lo,towerMd.s.Hi)*/);	//Send Tower Mode
}
bool SendVersion()
{
  return Packet_Put(0x09,'v',1,0);			//Send Tower Version V1.0
}
bool SendTowerNumber()
{
  return Packet_Put(0x0B,1,towerNb.s.Lo,towerNb.s.Hi);	//Send Tower Number
}
bool SetTowerNumber()
{
  towerNb.s.Lo = Packet_Parameter2;	//LSB
  towerNb.s.Hi = Packet_Parameter3;	//MSB
  return Packet_Put(0x0B,1,towerNb.s.Lo,towerNb.s.Hi);	//Send Tower Number
}
bool SendTowerMode()
{
  return Packet_Put(0x0D,1,towerMd.s.Lo,towerMd.s.Hi);	//Send Tower Mode
}
bool SetTowerMode()
{
  towerMd.s.Lo = Packet_Parameter2;	//LSB : 1 if synchronous, 0 if asynchronous
  towerMd.s.Hi = Packet_Parameter3;	//MSB : supposed to be 0
  return Packet_Put(0x0D,1,towerMd.s.Lo,towerMd.s.Hi);	//Send Tower Mode
}
bool ReadByte()
{

}

/*Acknowledgement and NonAcknowledgement functions*/
bool Packet_ACK()
{
  return Packet_Put(Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
}
bool Packet_NAK()
{
  return Packet_Put(PACKET_ACK_MASK|Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
}


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
  uint32_t baudRate = 38400;
  uint32_t moduleClk = CPU_BUS_CLK_HZ;

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */

  // Initialization of communication
  Packet_Init(baudRate, moduleClk);
  SendStartUpValues();

  for (;;)
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
