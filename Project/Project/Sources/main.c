/* ###################################################################
 **     Filename    : main.c
 **     Project     : Project
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
 ** @version 6.0
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

// Simple OS
#include "OS.h"

// Analog functions
#include "analog.h"

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
#include "Frequencie.h"
#include "Requirements.h"
#include "Regulation.h"

// ----------------------------------------
// Thread set up
// ----------------------------------------
// Arbitrary thread stack size - big enough for stacking of interrupts and OS use.
#define THREAD_STACK_SIZE 1024
#define NB_ANALOG_CHANNELS 3

// Thread stacks
OS_THREAD_STACK(InitModulesThreadStack, THREAD_STACK_SIZE); /*!< The stack for the LED Init thread. */
OS_THREAD_STACK(HandlePacketThreadStack, THREAD_STACK_SIZE);
OS_THREAD_STACK(TxThreadStack, THREAD_STACK_SIZE);
OS_THREAD_STACK(RxThreadStack, THREAD_STACK_SIZE);
OS_THREAD_STACK(RTCThreadStack, THREAD_STACK_SIZE);
OS_THREAD_STACK(PITThreadStack, THREAD_STACK_SIZE);
OS_THREAD_STACK(TakeSampleThreadStack, THREAD_STACK_SIZE);
OS_THREAD_STACK(ProcessSampleThreadStack, THREAD_STACK_SIZE);

// Thread stacks
OS_THREAD_STACK(InitModulesThreadStack, THREAD_STACK_SIZE); /*!< The stack for the LED Init thread. */
static uint32_t AnalogThreadStacks[NB_ANALOG_CHANNELS][THREAD_STACK_SIZE] __attribute__ ((aligned(0x08)));

// ----------------------------------------
// Thread priorities
// 0 = highest priority
// ----------------------------------------
const uint8_t ANALOG_THREAD_PRIORITIES[NB_ANALOG_CHANNELS] = {1, 2, 3};


//Timer declaration
TFTMChannel Timer1Sec;
//TFTMChannel Timer5SecLow;
//TFTMChannel Timer5SecRaise;

/*! @brief Data structure used to pass Analog configuration to a user thread
 *
 */
typedef struct AnalogThreadData
{
  OS_ECB* semaphore;
  uint8_t channelNb;
} TAnalogThreadData;

/*! @brief Analog thread configuration data
 *
 */
static TAnalogThreadData AnalogThreadData[NB_ANALOG_CHANNELS] =
{
  {
    .semaphore = NULL,
    .channelNb = 0
  },
  {
    .semaphore = NULL,
    .channelNb = 1
  }
};


/*! @brief Check if a command has been received and send the adequate answer
 *
 *  @param pData is not used but is required by the OS to create a thread.
 */
static void HandlePacketThread(void* pData)
{
  for(;;)
  {
//      OS_SemaphoreWait(Packet_Ready,0);
    if (Packet_Get())
    {
      LEDs_On(LED_BLUE);
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


/*! @brief Take one sample
 *
 *  @param pData is not used but is required by the OS to create a thread.
 */
static void PIT0Thread(void* pData)
{
  for(;;)
  {
    OS_SemaphoreWait(PIT0Access,0);
    //Toggle Green LED
    OS_SemaphoreSignal(SampleTaken);
    LEDs_Toggle(LED_GREEN);
  }
}

/*! @brief toggle green LED
 *
 *  @param pData is not used but is required by the OS to create a thread.
 */
static void PIT1Thread(void* pData)
{
  for(;;)
  {
    OS_SemaphoreWait(PIT1Access,0);
    //Toggle Green LED
    LEDs_Toggle(LED_GREEN);
    //Signal a variable
    Regulation_AlarmSet = true;
  }
}
///*! @brief raiser
// *
// *  @param pData is not used but is required by the OS to create a thread.
// */
//static void FTMTRaiserhread(void* pData)
//{
//  for(;;)
//  {
//    OS_SemaphoreWait(FTMAccess1Raise,0);
//    Output_SetRaise();
//  }
//}
//
///*! @brief lower
// *
// *  @param pData is not used but is required by the OS to create a thread.
// */
//static void FTMTLowerhread(void* pData)
//{
//  for(;;)
//  {
//    OS_SemaphoreWait(FTMAccess2Low,0);
//    Output_SetLower();
//  }
//}


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

void __attribute__ ((interrupt)) LPTimer_ISR(void)
{
  OS_ISREnter();

  // Clear interrupt flag
  LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;

  // Signal the analog channels to take a sample
  for (uint8_t analogNb = 0; analogNb < NB_ANALOG_CHANNELS; analogNb++)
    (void)OS_SemaphoreSignal(AnalogThreadData[analogNb].semaphore);

  OS_ISRExit();
}

/*! @brief Initialises modules.
 *
 */
static void InitModulesThread(void* pData)
{
  OS_DisableInterrupts();

  //Initialize variables
  SCP_TowerNb.l = 5605;
  SCP_TowerMd.l = 0;
  //Init Signal period in ms assuming a 50Hz signal
  *Regulation_Ts = 20;
  //Init timing mode
  SCP_RegMode = DEFINITE_TIMER;
  // Baud Rate and Module Clock
  uint32_t baudRate = 115200;
  uint32_t moduleClk = CPU_BUS_CLK_HZ;

  // Analog
  (void)Analog_Init(CPU_BUS_CLK_HZ);

  // Generate the global analog semaphores
  for (uint8_t analogNb = 0; analogNb < NB_ANALOG_CHANNELS; analogNb++)
    AnalogThreadData[analogNb].semaphore = OS_SemaphoreCreate(0);

  //Semaphores required for Voltage Regulation
  FullSampleTaken = OS_SemaphoreCreate(0);
  SampleTaken = OS_SemaphoreCreate(0);

  // Initialise the low power timer to tick every 5 ms
  LPTMRInit(5);

  //Init communication
  Packet_Init(baudRate, moduleClk);
  //Init LEDs
  LEDs_Init();
  //Init Timers
  FTM_Init();
  PIT_Init(moduleClk, NULL, NULL);
  RTC_Init(NULL, NULL);
  //Init Flash
  Flash_Init();

  //writing Tower number and mode in flash
  if(Flash_AllocateVar((volatile void**)&NvTowerNb, sizeof(*NvTowerNb)))
    if(NvTowerNb->l == 0xFFFF)
      Flash_Write16((volatile uint16_t *)NvTowerNb, SCP_TowerNb.l);

  if(Flash_AllocateVar((volatile void**)&NvTowerMd, sizeof(*NvTowerMd)))
    if(NvTowerMd->l == 0xFFFF)
      Flash_Write16((uint16_t *)NvTowerMd, SCP_TowerMd.l);


//  //Test PutSample
//  for(int i = 0; i < NB_OF_SAMPLE; i++)
//  {
//    int16_t data;
//    Analog_Get(0,&data);
//    TakeSample(FullSample, data);
//    OS_TimeDelay(4);
//  }

//  Analog_Put(0,5*3277);

//  //Start PIT for 1sec
//  PIT_Enable(false);
//  PIT_Set(1000000000,true);
//  PIT_Enable(true);
  //Start PIT for 1.25ms = TSignal / 16 *1000 to be in nanosec
//  PIT0_Enable(false);
  PIT0_Set((uint32_t)(*Regulation_Ts * 1000 / 16),true);
//  PIT0_Enable(true);

  //Create 1sec Timer with FTM
  Timer1Sec.channelNb = 0;  //arbitraire, faire attentiotn quand on les d�clare manuellement
  Timer1Sec.delayCount = CPU_MCGFF_CLK_HZ_CONFIG_0; //1sec
  Timer1Sec.ioType.outputAction = TIMER_OUTPUT_DISCONNECT;
  Timer1Sec.timerFunction = TIMER_FUNCTION_OUTPUT_COMPARE;
  Timer1Sec.userArguments = NULL;
  Timer1Sec.userFunction = NULL;
  FTM_Set(&Timer1Sec);

//  Timer5SecRaise.channelNb = 0;  //arbitraire, faire attentiotn quand on les d�clare manuellement
//  Timer5SecRaise.delayCount = CPU_MCGFF_CLK_HZ_CONFIG_0; //1sec
//  Timer5SecRaise.ioType.outputAction = TIMER_OUTPUT_DISCONNECT;
//  Timer5SecRaise.timerFunction = TIMER_FUNCTION_OUTPUT_COMPARE;
//  Timer5SecRaise.userArguments = NULL;
//  Timer5SecRaise.userFunction = NULL;
//  FTM_Set(&Timer5SecRaise);
//
//  Timer5SecLow.channelNb = 0;  //arbitraire, faire attentiotn quand on les d�clare manuellement
//  Timer5SecLow.delayCount = CPU_MCGFF_CLK_HZ_CONFIG_0; //1sec
//  Timer5SecLow.ioType.outputAction = TIMER_OUTPUT_DISCONNECT;
//  Timer5SecLow.timerFunction = TIMER_FUNCTION_OUTPUT_COMPARE;
//  Timer5SecLow.userArguments = NULL;
//  Timer5SecLow.userFunction = NULL;
//  FTM_Set(&Timer5SecLow);

  OS_EnableInterrupts();

  //Turn on orange LED
  LEDs_On(LED_ORANGE);
  //Send Start up values
  SCP_SendStartUpValues();

  // We only do this once - therefore delete this thread
  OS_ThreadDelete(OS_PRIORITY_SELF);
}

/*! @brief Samples a value on an ADC channel and sends it to the corresponding DAC channel.
 *
 */
void AnalogLoopbackThread(void* pData)
{
  // Make the code easier to read by giving a name to the typecast'ed pointer
  #define analogData ((TAnalogThreadData*)pData)

  for (;;)
  {
    int16_t analogInputValue;

    (void)OS_SemaphoreWait(analogData->semaphore, 0);
    // Get analog sample
    Analog_Get(analogData->channelNb, &analogInputValue);
    // Put analog sample
    Analog_Put(analogData->channelNb, analogInputValue);
  }
}

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  OS_ERROR error;

  // Initialise low-level clocks etc using Processor Expert code
  PE_low_level_init();

  // Initialize the RTOS
  OS_Init(CPU_CORE_CLK_HZ, true);

  // Create module initialisation thread
  error = OS_ThreadCreate(InitModulesThread,
                          NULL,
                          &InitModulesThreadStack[THREAD_STACK_SIZE - 1],
                          0); // Highest priority

  //Create Threads
   error = OS_ThreadCreate(UART_RxThread,
                           NULL,
                           &RxThreadStack[THREAD_STACK_SIZE - 1],
                           1);
   error = OS_ThreadCreate(UART_TxThread,
                           NULL,
                           &TxThreadStack[THREAD_STACK_SIZE - 1],
                           2); // Second Highest priority

   error = OS_ThreadCreate(Regulation_ProcessSampleThread,
                           NULL,
                           &ProcessSampleThreadStack[THREAD_STACK_SIZE - 1],
                           3);

   error = OS_ThreadCreate(Regulation_TakeSampleThread,
                           NULL,
                           &TakeSampleThreadStack[THREAD_STACK_SIZE - 1],
                           4);

   error = OS_ThreadCreate(HandlePacketThread,
                           NULL,
                           &HandlePacketThreadStack[THREAD_STACK_SIZE - 1],
                           6);
   error = OS_ThreadCreate(PIT0Thread,
                           NULL,
                           &PITThreadStack[THREAD_STACK_SIZE - 1],
                           7);
   error = OS_ThreadCreate(PIT1Thread,
                           NULL,
                           &PITThreadStack[THREAD_STACK_SIZE - 1],
                           5);
   error = OS_ThreadCreate(RTC_Thread,
                           NULL,
                           &RTCThreadStack[THREAD_STACK_SIZE - 1],
                           10);


  // Create threads for analog loopback channels
//  for (uint8_t threadNb = 0; threadNb < NB_ANALOG_CHANNELS; threadNb++)
//  {
//    error = OS_ThreadCreate(AnalogLoopbackThread,
//                            &AnalogThreadData[threadNb],
//                            &AnalogThreadStacks[threadNb][THREAD_STACK_SIZE - 1],
//                            ANALOG_THREAD_PRIORITIES[threadNb]);
//  }

  // Start multithreading - never returns!
  OS_Start();
}

/*!
 ** @}
 */
