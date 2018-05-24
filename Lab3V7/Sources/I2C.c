/*! @file I2C.c
 *
 *  @brief I/O routines for the K70 I2C interface.
 *
 *  This contains the functions for operating the I2C (inter-integrated circuit) module.
 *
 *  @author Harindi and Coralie
 *  @date 2018-05-16
 */

/*!
 **  @addtogroup I2C_module I2C module documentation
 **  @{
 */
/* MODULE I2C */


// new types
#include "types.h"
#include "Cpu.h"
#include "PE_Types.h"
#include "MK70F12.h"
#include "LEDs.h"
#include "I2C.h"
#include "stdlib.h"

/*Public Global Variables*/
#define MULT_SIZE 3
#define SCL_DIV_SIZE 64
#define READORWRITE 0x01

#define COMPLETE_READ 0
#define LAST_READ 1
#define SECOND_LAST_READ 2

/*Private Global Variables*/
static char SlaveAdress;
static uint8_t I2CReadSequence[3];
static uint8_t SequencePosition;
static uint8_t NumOfBytes;
static uint8_t* DataPtr;

/*used in ISR to keep track of read and write requested*/
static uint8_t DataCounter = 0;


/*The user call back function and argument globalised*/
static void (*ReadCompleteCallbackFunction)(void*);
static void* ReadCompleteCallbackArguments;

//function Prototypes
static void Start(void);
static void WaitforAck(void);
static void Stop(void);


bool I2C_Init(const TI2CModule* const aI2CModule, const uint32_t moduleClk)
{
  /*make call back functions and it's arugments gloablly available*/
  ReadCompleteCallbackFunction = aI2CModule->readCompleteCallbackFunction;
  ReadCompleteCallbackArguments= aI2CModule->readCompleteCallbackArguments;

  uint32_t I2Cbaudrate;
  uint32_t olderror = 1000000000; /*arbitary high value should this be in define?*/
  uint32_t newerror;

  /*2 variables that will point to the index*/
  uint8_t mult;
  uint16_t scldiv;

  /*Enable Port Clocks*/
  SIM_SCGC4 |= SIM_SCGC4_IIC0_MASK ;

  /*Enable OpenDrainmode*/
  PORTE_PCR18 |= PORT_PCR_MUX(4) | PORT_PCR_ODE_MASK; /*SDA data line open*/
  PORTE_PCR19 |= PORT_PCR_MUX(4) | PORT_PCR_ODE_MASK; /*SCL data line open*/


/**********************BaudRate Calculation*****************************************/
  //The 2 arrays that hold all the data*/
  const uint8_t Mult_fact[MULT_SIZE] = {1, 2, 4};

  const uint16_t SCL_Div[SCL_DIV_SIZE] = {20, 22, 24, 26, 28, 30, 34, 40, 28,
      32, 36, 40, 44, 48, 56, 68, 48, 56, 64, 72, 80,
      88, 104, 128, 80, 96, 112, 128, 144, 160, 192,
      240, 160, 192, 224, 256, 288, 320, 384, 480, 320, 384 , 448, 512, 576, 640, 768, 960, 640, 768, 896, 1024, 1152, 1280, 1536, 1920, 1280, 1536, 1792, 2048, 2304, 2560, 3072, 3840};


  /*Iterates through all possible multiplier values*/
  for (int i=0; i<MULT_SIZE; i++)
  {
    /*iterates through all SCL divider values*/
    for (int j=0; j<SCL_DIV_SIZE; j++)
    {
     //Calculates baud rate using multiplier factor and scl divider
      I2Cbaudrate = moduleClk/(Mult_fact[i] * SCL_Div[j]);
     //Checks for New error value
      newerror = abs(I2Cbaudrate - aI2CModule->baudRate);
      if (newerror < olderror)
      {
        //Trys to get the lowest possible error
        olderror = newerror;
       // Gets the index at this point of lowest error
        mult = Mult_fact[i];
        scldiv = SCL_Div[j];
      }
    }
  }

  /***********************************************************************************/
  /*set the Baud Rate*/
  I2C0_F = I2C_F_MULT(mult) | I2C_F_ICR(scldiv);

  /*Enable I2C*/
  I2C0_C1 = I2C_C1_IICEN_MASK;

  /*Enable NVIC by clearing pending request and enabling interupts*/
  NVICICPR0 = NVIC_ICPR_CLRPEND(1 << 24);
  NVICISER0 = NVIC_ISER_SETENA(1 << 24);

  return true;

}

void I2C_SelectSlaveDevice(const uint8_t slaveAddress)
{
  //needs to take this variable and make it a global variable
  SlaveAdress = slaveAddress;
}


static void Start(void)
{
  /*enable MST mode*/
  I2C0_C1 |= I2C_C1_MST_MASK;
  /*enable Transmit mode*/
  I2C0_C1 |= I2C_C1_TX_MASK;
}

static void WaitforAck(void)
{
  while (!((I2C0_S & I2C_S_IICIF_MASK) == I2C_S_IICIF_MASK))
  {}
  I2C0_S |= I2C_S_IICIF_MASK;
}

static void Stop(void)
{
  I2C0_C1 &= ~I2C_C1_MST_MASK;
}

void I2C_Write(const uint8_t registerAddress, const uint8_t data)
{
  /*check if bus is idle from status reg*/
  while ((I2C0_S & I2C_S_BUSY_MASK) == I2C_S_BUSY_MASK)
  {}
  /*Master transmits the Start condition*/
  Start();
  /*Master transmits the Slave address with the R/W bit set to 0*/
  I2C0_D = (SlaveAdress << 1) & ~READORWRITE;
  /*Waits for ACK()*/
  WaitforAck();
  /*Master transmits the reg address */
  I2C0_D = registerAddress;
  /*Waits for ACK()*/
  WaitforAck();
  /*Master transmits the data*/
  I2C0_D = data;
  /*waits for ACK()*/
  WaitforAck();
  /*Transmits stop signal*/
  Stop();
}


void I2C_PollRead(const uint8_t registerAddress, uint8_t* const data, const uint8_t nbBytes)
{
  EnterCritical();
  /*check status reg to see if bus is idle*/
  while ((I2C0_S & I2C_S_BUSY_MASK) == I2C_S_BUSY_MASK)
  {}

  /*start in master transmit mode*/
  Start();
  /*send slaveaddress with write bit*/
  I2C0_D = (SlaveAdress << 1) & ~READORWRITE;
  /*wait*/
  WaitforAck();
  /*send slave register adress*/
  I2C0_D = registerAddress;
  /*wait*/
  WaitforAck();
  /*repeated start condition*/
  I2C0_C1 |= I2C_C1_RSTA_MASK;
  /*send slave adress with read bit now*/
  I2C0_D = (SlaveAdress << 1) | READORWRITE;
  /*wait*/
  WaitforAck();
  /*Switch on receieve mode*/
  I2C0_C1 &= ~I2C_C1_TX_MASK;
  //Different cases depending on the nb of bytes
  if (nbBytes == 1)
  {
    /*nACk from master*/
    I2C0_C1 |= I2C_C1_TXAK_MASK;
    /*dummy read*/
    data[0] = I2C0_D;
    WaitforAck();
    Stop();
    /*Read actual data*/
    data[0] = I2C0_D;
  }
  else if (nbBytes == 2)
  {
    /* ACK from master*/
    I2C0_C1 &= ~I2C_C1_TXAK_MASK;
    /*dummy read*/
    data[0] = I2C0_D;
    WaitforAck();
    /*activate transmission ack or - NAK from Master*/
    I2C0_C1 |= I2C_C1_TXAK_MASK;
    /*read second last byte and wait*/
    data[nbBytes-2] = I2C0_D;
    WaitforAck();
    /*stop and then read last byte*/
    Stop();
    data[nbBytes-1] = I2C0_D;
  }
  else
  {
    /*counter for counting how many data bytes more than 3*/
    uint8_t counter;
    /*read data from regs*/
    for (counter = 0; counter < nbBytes - 2 ; counter++)
    {
      data[counter] = I2C0_D;
      WaitforAck();
    }
    /*activate transmission ack or - NAK from Master*/
    I2C0_C1 |= I2C_C1_TXAK_MASK;
    /*read second last byte and wait*/
    data[nbBytes-2] = I2C0_D;
    WaitforAck();
    /*stop and then read last byte*/
    Stop();
    data[nbBytes-1] = I2C0_D;
  }
  ExitCritical();
}


//Pete said we can start like poll, and then we are supposed to do something else (and dont send the slave address x))
void I2C_IntRead(const uint8_t registerAddress, uint8_t* const data, const uint8_t nbBytes)
{
  EnterCritical();
  while ((I2C0_S & I2C_S_BUSY_MASK) == I2C_S_BUSY_MASK) // Wait till bus is idle
  {}
  I2C0_S |= I2C_S_IICIF_MASK; // Clear interrupt flag
  I2C0_C1 |= I2C_C1_IICIE_MASK; // I2C interrupt enable
//  DataPtr = data; // Array to store values into, is made globally accessible
//  NumOfBytes = nbBytes; // Number of bytes to read
//  I2CReadSequence[0] = (SlaveAdress << 1) & ~READORWRITE; // Send slave address with write bit
//  I2CReadSequence[1] = registerAddress; // Register Address
//  I2CReadSequence[2] = (SlaveAdress << 1) | READORWRITE;// Send slave address with read bit
//  SequencePosition = 0; // Initialize position

  /*start in master transmit mode*/
  Start();
  /*send slaveaddress with write bit*/
  I2C0_D = (SlaveAdress << 1) & ~READORWRITE;
  /*wait*/
  WaitforAck();
  /*send slave register adress*/
  I2C0_D = registerAddress;
  /*wait*/
  WaitforAck();
  /*repeated start condition*/
  I2C0_C1 |= I2C_C1_RSTA_MASK;
  /*send slave adress with read bit now*/
  I2C0_D = (SlaveAdress << 1) | READORWRITE;
  /*wait*/
//  WaitforAck();	//if we leave that, the interrupt flag is cleared and we dont enter the ISR which is BAD
  /*Switch on receieve mode*/
  I2C0_C1 &= ~I2C_C1_TX_MASK;
  ExitCritical();
//  Start(); // Start signal
//  I2C0_D = I2CReadSequence[0]; // Send slave address with write bit

}


void __attribute__ ((interrupt)) I2C_ISR(void)
{
  I2C0_S |= I2C_S_IICIF_MASK; // Clear interrupt flag

  if (I2C0_S & I2C_S_BUSY_MASK) // Bus is busy
  {
    if (I2C0_C1 & I2C_C1_TX_MASK) // In transmit mode
    {
	/*No receive interrupt needed according to the requirements*/

//        if (!(I2C0_S & I2C_S_RXAK_MASK)) // Check if ACK received
//        {
//          SequencePosition++; // Move to next item in sequence
//          if (SequencePosition == 2)
//          {
//            I2C0_C1 |=  I2C_C1_RSTA_MASK; // Restart signal
//            I2C0_D = I2CReadSequence[SequencePosition]; // Send slave address with read bit
//          }
//          else if (SequencePosition == 3)
//          {
//            I2C0_C1 &= ~I2C_C1_TX_MASK; // Receive mode
//          I2C0_C1 &= ~I2C_C1_TXAK_MASK; // Turn on ACK from master
//          DataPtr[DataCounter] = I2C0_D;
//          }
//          else
//          {
//            I2C0_D = I2CReadSequence[SequencePosition]; // Send slave register address
//          }
//          return;
//        }
//        else // No ACK received
//        {
//          Stop(); // Stop signal
//          return;
//        }
    }
    //maybe dpo 3 cases like poll read
    else if (!(I2C0_C1 & I2C_C1_TX_MASK) && (DataCounter < NumOfBytes)) // In receive mode
    {
      if (DataCounter == (NumOfBytes -1)) // Check if last byte to be read
      {
	Stop();
	DataPtr[DataCounter] = I2C0_D; // Read last byte
	DataCounter = 0; // Reset data counter
	SequencePosition = 0; // Reset position counter

	I2C0_C1 &= ~I2C_C1_IICIE_MASK; // I2C interrupt disable
	I2C0_S |= I2C_S_IICIF_MASK; // Clear interrupt flag

	(*ReadCompleteCallbackFunction)(ReadCompleteCallbackArguments); // Accelerometer read complete callback function
	return;
      }
      else if (DataCounter == (NumOfBytes -2)) // Check if second last byte to be read
      {
	I2C0_C1 |= I2C_C1_TXAK_MASK; // NACK from master
      }
      DataPtr[DataCounter] = I2C0_D; // Read data
      DataCounter++; // Increment data counter
    }
  }
}


/*!
 ** @}
 */
