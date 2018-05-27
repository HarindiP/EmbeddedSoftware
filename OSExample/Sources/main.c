/*!
** @file
** @version 1.0
** @brief  Main module.
**
**   This file contains the high-level code for the project.
**   It initialises appropriate hardware subsystems,
**   creates application threads, and then starts the OS.
**
**   An example of two threads communicating via a semaphore
**   is given that flashes the orange LED. These should be removed
**   when the use of threads and the RTOS is understood.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


// CPU module - contains low level hardware initialization routines
#include "Cpu.h"

// Simple OS
#include "OS.h"

//Sources module
#include "types.h"
#include "UART.h"
#include "packet.h"
#include "FIFO.h"
#include "SCP.h"
#include "LEDs.h"
#include "PIT.h"
#include "RTC.h"
#include "FTM.h"

// Arbitrary thread stack size - big enough for stacking of interrupts and OS use.
#define THREAD_STACK_SIZE 1024

// Thread stacks
OS_THREAD_STACK(InitModulesThreadStack, THREAD_STACK_SIZE); /*!< The stack for the LED Init thread. */
OS_THREAD_STACK(HandlePacketThreadStack, THREAD_STACK_SIZE);
OS_THREAD_STACK(TxThreadStack, THREAD_STACK_SIZE);
OS_THREAD_STACK(RxThreadStack, THREAD_STACK_SIZE);



///*! @brief Data structure used to pass LED configuration to a user thread
// *
// */
//typedef struct LEDThreadData
//{
//  OS_ECB* semaphore;
//  TLED color;
//  uint8_t delay;
//  struct LEDThreadData* next;
//} TLEDThreadData;


//Timer declaration
TFTMChannel Timer1Sec;

//Communication thread
static void HandlePacketThread(void* pData)
{
  for(;;)
  {
    if (Packet_Get())
    {
      LEDs_Toggle(LED_BLUE);
      FTM_StartTimer(&Timer1Sec);
      if(!SCP_Acknowledgement_Required(Packet_Command))   //Cases without Packet Acknowledgement required
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

void LPTMRInit(const uint16_t count)
{
  // Enable clock gate to LPTMR module
  SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;

  // Disable the LPTMR while we set up
  // This also clears the CSR[TCF] bit which indicates a pending interrupt
  LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;

  // Enable LPTMR interrupts
  LPTMR0_CSR |= LPTMR_CSR_TIE_MASK;
  // Reset the LPTMR free running counter whenever the 'counter' equals 'compare'
  LPTMR0_CSR &= ~LPTMR_CSR_TFC_MASK;
  // Set the LPTMR as a timer rather than a counter
  LPTMR0_CSR &= ~LPTMR_CSR_TMS_MASK;

  // Bypass the prescaler
  LPTMR0_PSR |= LPTMR_PSR_PBYP_MASK;
  // Select the prescaler clock source
  LPTMR0_PSR = (LPTMR0_PSR & ~LPTMR_PSR_PCS(0x3)) | LPTMR_PSR_PCS(1);

  // Set compare value
  LPTMR0_CMR = LPTMR_CMR_COMPARE(count);

  // Initialize NVIC
  // see p. 91 of K70P256M150SF3RM.pdf
  // Vector 0x65=101, IRQ=85
  // NVIC non-IPR=2 IPR=21
  // Clear any pending interrupts on LPTMR
  NVICICPR2 = NVIC_ICPR_CLRPEND(1 << 21);
  // Enable interrupts from LPTMR module
  NVICISER2 = NVIC_ISER_SETENA(1 << 21);

  //Turn on LPTMR and start counting
  LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;
}


/*! @brief Initialises the modules to support the LEDs and low power timer.
 *
 *  @param pData is not used but is required by the OS to create a thread.
 *  @note This thread deletes itself after running for the first time.
 */
static void InitModulesThread(void* pData)
{
  OS_DisableInterrupts();

  // Baud Rate and Module Clock
  uint32_t baudRate = 115200;
  uint32_t moduleClk = CPU_BUS_CLK_HZ;

  //Init communication
  Packet_Init(baudRate, moduleClk);
  //Init LEDs
  LEDs_Init();
  //Init Timer
  FTM_Init();
  PIT_Init(moduleClk, NULL, NULL);
  RTC_Init(NULL, NULL);

  //Generate semaphores
//PacketReady = OS_SemaphoreCreate(0);

  OS_EnableInterrupts();

  //Initialize variables
  SCP_TowerNb.l = 5605;
  SCP_TowerMd.l = 0;

  //Start PIT for 1sec
  PIT_Enable(false);
  PIT_Set(1000000000,true);
  PIT_Enable(true);
  //Create 1sec Timer with FTM
  Timer1Sec.channelNb = 0;  //arbitraire, faire attentiotn quand on les déclare manuellement
  Timer1Sec.delayCount = CPU_MCGFF_CLK_HZ_CONFIG_0; //1sec
  Timer1Sec.ioType.outputAction = TIMER_OUTPUT_DISCONNECT;
  Timer1Sec.timerFunction = TIMER_FUNCTION_OUTPUT_COMPARE;
  Timer1Sec.userArguments = NULL;
  Timer1Sec.userFunction = NULL;
  FTM_Set(&Timer1Sec);

  //Turn on orange LED
  LEDs_On(LED_ORANGE);
  //Send Start up values
  SCP_SendStartUpValues();

  // We only do this once - therefore delete this thread
  OS_ThreadDelete(OS_PRIORITY_SELF);
}

void __attribute__ ((interrupt)) LPTimer_ISR(void)
{
  // Clear interrupt flag
  LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;
  // Signal the orange LED to toggle
//  (void)OS_SemaphoreSignal(MyLEDThreadData[3].semaphore);
}

/*! @brief Waits for a signal to toggle the LED, then waits for a specified delay, then signals for the next LED to toggle.
 *
 *  @param pData holds the configuration data for each LED thread.
 *  @note Assumes that LEDInit has been called successfully.
 */
//static void LEDThread(void* pData)
//{
//  // Make the code easier to read by giving a name to the typecast'ed pointer
////  #define ledData ((TLEDThreadData*)pData)
//
//  for (;;)
//  {
//    // Wait here until signaled that we can turn the LED on
//    (void)OS_SemaphoreWait(ledData->semaphore, 0);
//
//    // Toggle LED
//    GPIOA_PTOR = ledData->color;
//
//    // Wait for the required toggle time if the delay > 0
//    if (ledData->delay)
//      OS_TimeDelay(ledData->delay);
//
//    // Signal for the next LED to toggle
//    if (ledData->next->semaphore)
//      (void)OS_SemaphoreSignal(ledData->next->semaphore);
//  }
//}



/*! @brief Initialises the hardware, sets up threads, and starts the OS.
 *
 */
/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  OS_ERROR error;
  // Initialise low-level clocks etc using Processor Expert code
  PE_low_level_init();
  // Initialize the RTOS - without flashing the orange LED "heartbeat"
  OS_Init(CPU_CORE_CLK_HZ, false);

  // Create module initialisation thread
  error = OS_ThreadCreate(InitModulesThread,
                          NULL,
                          &InitModulesThreadStack[THREAD_STACK_SIZE - 1],
			  0); // Highest priority

  //Create Com Thread
  error = OS_ThreadCreate(RxThread,
                          NULL,
                          &RxThreadStack[THREAD_STACK_SIZE - 1],
			  1); //Highest priority
  error = OS_ThreadCreate(TxThread,
                          NULL,
                          &TxThreadStack[THREAD_STACK_SIZE - 1],
			  2); // Second Highest priority
  error = OS_ThreadCreate(HandlePacketThread,
                          NULL,
                          &HandlePacketThreadStack[THREAD_STACK_SIZE - 1],
			  3);

  // Create threads to toggle the LEDS
//  for (uint8_t threadNb = 0; threadNb < NB_LEDS; threadNb++)
//  {
//    error = OS_ThreadCreate(LEDThread,
//	                    &MyLEDThreadData[threadNb],
//		            &MyLEDThreadStacks[threadNb][THREAD_STACK_SIZE - 1],
//			    LED_THREAD_PRIORITIES[threadNb]);
//  }

  // Start multithreading - never returns!
  OS_Start();
}

/*!
** @}
*/
