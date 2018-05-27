/*! @file FTM.c
 *
 *  @brief Routines for setting up the FlexTimer module (FTM) on the TWR-K70F120M.
 *
 *  This contains the functions for operating the FlexTimer module (FTM).
 *
 *  @author Coraie&Harindi
 *  @date 2018-05-27
 */
/*!
 **  @addtogroup FTM_module FTM module documentation
 **  @{
 */
/* MODULE FTM */

#include "types.h"
#include "FTM.h"
#include "Cpu.h"
#include "PE_Types.h"
#include "LEDs.h"

static void (*UserFunctions[8])(void*);
static void (*UserArguments[8]);

//The clock source for the FTM should be the fixed frequency clock (MCGFFCLK).

bool FTM_Init()
{
  EnterCritical();
  /*Write to CNTIN.
  Write to MOD.
  Write to CNT.
  Write to CLKS[1:0] (in SC).*/
  /*Initialize the FTM counter, by writing to CNT, before writing to the MOD register to
avoid confusion about when the first counter overflow will occur. (p1218)*/
  //???

  /*The FTM counter clock is the selected clock divided by the prescaler.*/
  SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;
  //Reset statues reg
  FTM0_SC = 0;


  /* Disable write protection for FlexTimer 0 using the FTM0_MODE register
   * (Features Mode Selection register for FlexTimer 0) (See 44.3.10 on
   * page 1240 of the K70 Sub-Family Reference Manual, Rev. 2, Dec 2011) */
//  FTM0_MODE = FTM_MODE_WPDIS_MASK;

  //Write the CNTIN value
  FTM0_CNTIN = 0;	//down value
  //Writing the MOD value
  FTM0_MOD = 0xFFFF;		//upper value
  //Reset counter
  //FTM0_CNT |= FTM_CNTIN_INIT_MASK;
  FTM0_CNT = 0;

  //define clock source
  //FTM0_SC |= (1<<5);
//  FTM0_SC |= FTM_SC_CLKS(0);
//  //Define prescaler
//  FTM0_SC |= (1<<0); 	//prescaler on 3 bits : 0b001 //7bit p1260 ??
//  FTM0_SC |= FTM_SC_PS(0); //no needs cause MGCFFCLK is already 1sec
//
//  //Enable interrupts
//  FTM0_SC |= FTM_SC_TOIE_MASK;

  FTM0_SC &= ~FTM_SC_TOIE_MASK;
  // Operate the timer in Up Counting mode
  FTM0_SC &= ~FTM_SC_CPWMS_MASK;
  // Use the system fixed frequency clock for the counter (24.414 kHz)
  FTM0_SC |= FTM_SC_CLKS(2);
  // Set the prescale value to 1
  FTM0_SC = (FTM0_SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(0);


  FTM0_MODE |= FTM_MODE_FTMEN_MASK;
  //NVICEnableIRQ(FTM0_IRQ_NUMBER, FTM0_INTERRUPT_PRIORITY);
  //NVIC : Vector =78, IRQ = 62, NVIC non-IRP reg = 1, NVIC IRP reg = 15
  //reset pending FTM interrupts
  NVICICPR1 = (1 << (62 % 32));
  //enable FTM interrupts
  NVICISER1 = (1 << (62 % 32));

  ExitCritical();
  return true;

}


bool FTM_Set(const TFTMChannel* const aFTMChannel)
{
  /*The Output Compare mode is selected when:
  • DECAPEN = 0
  • COMBINE = 0
  • CPWMS = 0, and
  • MSnB:MSnA = 0:1
  Do that for all 8 channels p1268*/
  int mode;

  if(aFTMChannel->channelNb <0 || aFTMChannel->channelNb > 7)
  {
    return false;
  }

  //checks if timerFunction is Output Compare
  //aFTMChannel->timerFunction = TIMER_FUNCTION_OUTPUT_COMPARE;
  if(aFTMChannel->timerFunction == TIMER_FUNCTION_OUTPUT_COMPARE)
  {
      FTM0_CnSC(aFTMChannel->channelNb) = (FTM0_CnSC(aFTMChannel->channelNb) & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) | (aFTMChannel->ioType.outputAction << FTM_CnSC_ELSA_SHIFT);
      FTM0_CnSC(aFTMChannel->channelNb) |= FTM_CnSC_MSA_MASK;
//      mode = aFTMChannel->ioType.outputAction;
//      //checks which configuration are set in ioType
//      switch(mode)
//      {
//	case TIMER_OUTPUT_TOGGLE:
//	    FTM0_CnSC(aFTMChannel->channelNb) &= ~FTM_CnSC_ELSB_MASK; //configuration 01
//	    FTM0_CnSC(aFTMChannel->channelNb) |= FTM_CnSC_ELSA_MASK;
//	    break;
//	case TIMER_OUTPUT_LOW:
//	    FTM0_CnSC(aFTMChannel->channelNb) |= FTM_CnSC_ELSB_MASK; //configuration 10
//	    FTM0_CnSC(aFTMChannel->channelNb) &= ~FTM_CnSC_ELSA_MASK;
//	    break;
//	case TIMER_OUTPUT_HIGH:
//	    FTM0_CnSC(aFTMChannel->channelNb) |= FTM_CnSC_ELSB_MASK; //configuration 11
//	    FTM0_CnSC(aFTMChannel->channelNb) |= FTM_CnSC_ELSA_MASK;
//	    break;
//	default:
//	    FTM0_CnSC(aFTMChannel->channelNb) &= ~FTM_CnSC_ELSB_MASK; //configuration 00
//	    FTM0_CnSC(aFTMChannel->channelNb) &= ~FTM_CnSC_ELSA_MASK;
//	    break;
//      }
//      FTM0_MOD = aFTMChannel->delayCount;	//safe ?
  }
  UserFunctions[aFTMChannel->channelNb] = aFTMChannel->userFunction;
  UserArguments[aFTMChannel->channelNb] = aFTMChannel->userArguments;

  return true;

}


bool FTM_StartTimer(const TFTMChannel* const aFTMChannel)
{
  /*Up counting is selected when:
  • QUADEN = 0, and
  • CPWMS = 0
  when reach MOD, set TOF and go back to CNTIN
  NUMTOF = 0*/
  /* Enable FTM overflow interrupts, Up counting mode,
   * Select the Fixed Frequency Clock, Prescaler to divide by 32 --> 6 */
//  FTM0_SC = FTM_SC_TOIE_MASK |
//	    FTM_SC_CLKS(2) |		//25000000U Hz
//	    FTM_SC_PS(0);	//no needs
  if (aFTMChannel->channelNb < 8)
  {
    if(aFTMChannel->timerFunction == TIMER_FUNCTION_OUTPUT_COMPARE)
    {
      //reset the channel flag
      FTM0_CnSC(aFTMChannel->channelNb) &= ~FTM_CnSC_CHF_MASK;
      //write the 1 second delay
      FTM0_CnV(aFTMChannel->channelNb) = FTM0_CNT + aFTMChannel->delayCount;
      //enables the channel's interrupt
      FTM0_CnSC(aFTMChannel->channelNb) |= FTM_CnSC_CHIE_MASK;
    }
    return true;
  }
  return false;

}


void __attribute__ ((interrupt)) FTM0_ISR(void)
{
  /*//Clear pending TOF
  FTM0_SC;
  //Clear TOF
  FTM0_SC |= FTM_SC_TOIE_MASK;
  FTM0_SC = FTM_SC_TOIE_MASK |
    FTM_SC_CLKS(2) |	//MGCFFCLK
    FTM_SC_PS(/*value);
  //Act
  FTM_CallbackFunction();*/

  //Disable timer
//  FTM0_SC &= ~FTM_SC_CLKS_MASK;
//  FTM0_CNT |= FTM_CNT_COUNT_MASK;

  for(uint8_t channel = 0; channel < 8; channel++)
  {
    //check each channel to see if its flag and interrupt is set
    if((FTM0_CnSC(channel) & FTM_CnSC_CHF_MASK) && (FTM0_CnSC(channel) & FTM_CnSC_CHIE_MASK))
    {
      FTM0_CnSC(channel) &= ~FTM_CnSC_CHF_MASK;		//reset flag
      FTM0_CnSC(channel) &= ~FTM_CnSC_CHIE_MASK;
//      if (UserFunctions[channel])
//	(*UserFunctions[channel])(UserArguments[channel]);
      LEDs_Off(LED_BLUE);
    }
  }
}

/*!
 ** @}
 */
