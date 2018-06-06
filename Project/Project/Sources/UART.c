/*
 * UART.c
 *
 *  Created on: 6 Apr 2018
 *      Author: 13115605
 */
/*!
 **  @addtogroup UART_module UART module documentation
 **  @{
 */
/* MODULE UART */

// CPU module - contains low level hardware initialization routines
#include "Cpu.h"
//#include "Events.h"
//#include "PE_Types.h"
//#include "PE_Error.h"
//#include "PE_Const.h"
//#include "IO_Map.h"

//Sources module
#include "types.h"
#include "UART.h"
#include "packet.h"
#include "FIFO.h"

OS_ECB* TxAccess;
OS_ECB* RxAccess;


static TFIFO TxFIFO, RxFIFO;
//static TFIFO *const TxPtr = &TxFIFO;
//static TFIFO *const RxPtr = &RxFIFO;

/*! @brief Sets up the UART interface before first use.
 *
 *  @param baudRate The desired baud rate in bits/sec.
 *  @param moduleClk The module clock rate in Hz
 *  @return bool - TRUE if the UART was successfully initialized.
 */
bool UART_Init(const uint32_t baudRate, const uint32_t moduleClk)
{
    /*BaudRate settings*/
    uint16union_t SBR;
    uint8_t brfd;

    /*Turn UART2 on*/
    SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;
    /*Activate PORTE*/
    SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;

    /* Tx Pin */
    PORTE_PCR16 = PORT_PCR_MUX(3);
    /* Rx Pin */
    PORTE_PCR17 = PORT_PCR_MUX(3);

    //TO set BLPE mode write into regc : page 679

    /* C1[CLK] = 10
     * C1[IREFS] = 0
     * C2[LP] = 1
     */

    // For 38400 lab 1 0 - 34 - 0
    // For 115200 Lab 2 0 - 13 - 18

    SBR.l =  2 * moduleClk/baudRate;

    brfd  = (uint8_t)(SBR.l % 32);

    SBR.l = (uint16_t)(SBR.l / 32);

    /*Disable Transmitter and Receiver*/
    UART2_C2 &= ~UART_C2_RE_MASK;
    UART2_C2 &= ~UART_C2_TE_MASK;

    /*High half of the new value goes to BDH*/
    UART2_BDH = UART_BDH_SBR(SBR.s.Hi);
//    UART2_BDH = UART_BDH_SBR(0);

    /*The remainder writes to BDL*/
    UART2_BDL = UART_BDL_SBR(SBR.s.Lo);
//    UART2_BDL = UART_BDL_SBR(34);

    /*set UART2_C4 to brfd	*/
    UART2_C4 = UART_C4_BRFA(brfd);
//    UART2_C4 = UART_C4_BRFA(4);

    /*Initialization of Transmit Watermark and Receive Watermark to the size of a complete packet*/
    /*UART2_TWFIFO |= 40;
    UART2_RWFIFO |= 40;*/

    /*Enable Transmitter and Receiver*/
    UART2_C2 |= UART_C2_RE_MASK;
    UART2_C2 |= UART_C2_TE_MASK;

    /*Initialising Transmiter and Reciever Fifo*/
    FIFO_Init(&TxFIFO);
    FIFO_Init(&RxFIFO);

    //Enable interrupt
    UART2_C2 |= UART_C2_RIE_MASK;
//    UART2_C2 |= UART_C2_TIE_MASK;

    /*RESETS AND Enable NVIC*/
    /*NVICPT pending where IQR49 IPR12 */
    NVICICPR1 = (1 << (17));
    /*NVIC Enable interupts NVIC ICER*/
    NVICISER1 = (1 << (17));

    TxAccess = OS_SemaphoreCreate(0);
    RxAccess = OS_SemaphoreCreate(0);

    return true;

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
bool UART_InChar(uint8_t * const dataPtr)
{
    FIFO_Get(&RxFIFO,dataPtr);
    return true;
}

/*! @brief Put a byte in the transmit FIFO if it is not full.
 *
 *  @param data The byte to be placed in the transmit FIFO.
 *  @return bool - TRUE if the data was placed in the transmit FIFO.
 *  @note Assumes that UART_Init has been called.
 */
bool UART_OutChar(const uint8_t data)
{
    FIFO_Put(&TxFIFO,data);
    //Enable Interrupt YES
    UART2_C2 |= UART_C2_TIE_MASK;
    return true;
}

/*! @brief Poll the UART status register to try and receive and/or transmit one character.
 *
 *  @return void
 *  @note Assumes that UART_Init has been called.
 */
void UART_Poll(void)
{
  uint8_t statusReg = UART2_S1;

  /*56.3.5*/
  if ((statusReg & UART_S1_RDRF_MASK) != 0)
  {
    FIFO_Put(&RxFIFO, UART2_D); //Receives one bit
  }
  /*Clear flag by reading register*/
  if ((statusReg & UART_S1_TDRE_MASK) != 0 )
  {
    FIFO_Get(&TxFIFO, (uint8_t *)&UART2_D); //Transmits one bit
  }
}

//Transmitting Thread
void UART_TxThread(void* pData)
{
  for(;;)
  {
    OS_SemaphoreWait(TxAccess,0);
    //Why do we have to do that ?
    if (UART2_S1 & UART_S1_TDRE_MASK)
    {
      FIFO_Get(&TxFIFO, (uint8_t *)&UART2_D);//Transmits one byte
    }
    UART2_C2 |= UART_C2_TIE_MASK;
  }
}

uint8_t RxData; //maybe not here

//Receiving Thread
void UART_RxThread(void* pData)
{
  for(;;)
  {
    OS_SemaphoreWait(RxAccess,0);
    FIFO_Put(&RxFIFO, RxData);
//    UART2_C2 |= UART_C2_RIE_MASK;
//    if(Packet_Get())
//    {
//      OS_SemaphoreSignal(Packet_Ready);	//TODO : why this doesnt work ???
//    }
  }
}


void __attribute__ ((interrupt)) UART_ISR(void)
{
  OS_ISREnter();
  //Receive throught interrupt
  if(UART2_C2 & UART_C2_RIE_MASK)
  {
    if (UART2_S1 & UART_S1_RDRF_MASK)
    {
      //Disbale interrupt until byte red
//      UART2_C2 &= ~UART_C2_RIE_MASK;
      RxData = UART_D;
      //Enter thread
      OS_SemaphoreSignal(RxAccess);
    }
  }
  //Transmit throught interrupt
  if(UART2_C2 & UART_C2_TIE_MASK)
  {
    if (UART2_S1 & UART_S1_TDRE_MASK)
    {
	//Desable interrupt until byte has been sent
	UART2_C2 &= ~UART_C2_TIE_MASK;
	//Enter thread
    	OS_SemaphoreSignal(TxAccess);
    }
  }
  OS_ISRExit();
}

/*!
 ** @}
 */
