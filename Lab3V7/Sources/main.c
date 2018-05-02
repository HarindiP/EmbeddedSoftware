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
#include "PIT.h"
#include "RTC.h"
#include "FTM.h"


//Timer declaration
TFTMChannel Timer1Sec;

//Interruption CallBack functions
void PITCallback(void* arg)
{
  //Toggle Green LED
  LEDs_Toggle(LED_GREEN);
}
void RTCCallback(void* arg)
{
  //Toggle Yellow LED
  LEDs_Toggle(LED_YELLOW);
  //Send Time
  SendTime();
}
void FTMCallback(void* arg)
{
  LEDs_Toggle(LED_BLUE);
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
  uint32_t baudRate = 115200;
  uint32_t moduleClk = CPU_BUS_CLK_HZ;

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */

  // Initialization of communication
  if (Packet_Init(baudRate, moduleClk) && Flash_Init() && LEDs_Init() && FTM_Init()
	 && PIT_Init(moduleClk, PITCallback, NULL) && RTC_Init(RTCCallback, NULL))
  {
    Timer1Sec.channelNb = 0;	//arbitraire, faire attentiotn quand on les déclare manuellement
    Timer1Sec.delayCount = CPU_MCGFF_CLK_HZ_CONFIG_0;	//1sec
    Timer1Sec.ioType.outputAction = TIMER_OUTPUT_DISCONNECT;
    Timer1Sec.timerFunction = TIMER_FUNCTION_OUTPUT_COMPARE;
    Timer1Sec.userArguments = NULL;
    Timer1Sec.userFunction = FTMCallback;
    FTM_Set(&Timer1Sec);

    bool success = true;

    //writing tower number and mode in flash
    if(Flash_AllocateVar((volatile void**)&NvTowerNb, sizeof(*NvTowerNb)))
      if(NvTowerNb->l == 0xFFFF)
	success = success && Flash_Write16((volatile uint16_t *)NvTowerNb, 5605);

    if(Flash_AllocateVar((volatile void**)&NvTowerMd, sizeof(*NvTowerMd)))
      if(NvTowerMd->l == 0xFFFF)
	success = success && Flash_Write16((uint16_t *)NvTowerMd, 1);

    if(success)
    {
      //light on the orange LED
      LEDs_On(LED_ORANGE);
      //sending start up values
      SCP_SendStartUpValues();

      for (;;)	//Should we put that in the previous if loop ?
      {

	  /*Checks the status of the serial port*/
	  //UART_Poll();
	  /*If we have a packet, we can check Serial Protocol Commands */
	  if(Packet_Get())
	  {
	    LEDs_On(LED_BLUE);
	    FTM_StartTimer(&Timer1Sec);
	    if(!Packet_Acknowledgement_Required(Packet_Command))		/*Cases without Packet Acknowledgement required*/
	    {
	      SCP_Packet_Handle();
	    }
	    else
	    {
	      SCP_Packet_Handle_Ack();
	    }
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
