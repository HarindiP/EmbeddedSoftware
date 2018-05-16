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


/*Private Global Variables*/
static char SlaveAdress;
static uint8_t I2CReadOrder[3];
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
  /*counter for counting how many data bytes*/
  uint8_t counter;

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
  /*Turn on ACk from master*/
  I2C0_C1 &= ~I2C_C1_TXAK_MASK;


  /*dummy read*/
  data[0] = I2C0_D;
  WaitforAck();


  /*create for loop reads until second as bit*/
  for (counter = 0; counter < nbBytes -2 ; counter++)
  {
    data[counter] = I2C0_D;
    WaitforAck();
  }

  /*activate tranmision ack or - NAK from Master*/
  I2C0_C1 |= I2C_C1_TXAK_MASK;

  /*read second last byte and wait*/
  data[nbBytes - 2] = I2C0_D;
  WaitforAck();

  /*stop and then read last byte*/
  Stop();
  data[nbBytes - 1] = I2C0_D;

}

void I2C_IntRead(const uint8_t registerAddress, uint8_t* const data, const uint8_t nbBytes)
{
  /*check to see if bus is idle*/
  while ((I2C0_S & I2C_S_BUSY_MASK) == I2C_S_BUSY_MASK) // Wait till bus is idle
  {}

  /*clear interupts flag*/
  I2C0_S |= I2C_S_IICIF_MASK;

  /*enable interupts*/
  I2C0_C1 |= I2C_C1_IICIE_MASK;

  /*Global Array/pointers that will store values and number of bytes*/
  DataPtr = data;
  NumOfBytes = nbBytes;

  /*the 3 step sequence */
  I2CReadOrder[0] = (SlaveAdress << 1) & ~READORWRITE;
  I2CReadOrder[1] = registerAddress;
  I2CReadOrder[2] = (SlaveAdress << 1) | READORWRITE;

  /*Initialise the position of the order*/
  SequencePosition = 0;

  /*Send start signal and slave adress with the write bit*/
  Start();
  I2C0_D = I2CReadOrder[0];
}

void __attribute__ ((interrupt)) I2C_ISR(void)
{
  // Clear interrupt flag
  I2C0_S |= I2C_S_IICIF_MASK;

  // check to see if Bus is busy
  if (I2C0_S & I2C_S_BUSY_MASK)
  {
    //check to see if In transmit mode
    if (I2C0_C1 & I2C_C1_TX_MASK)
    {
      // Check to see if ACK received
      if (!(I2C0_S & I2C_S_RXAK_MASK))
      {
        //increment the position to go to the next
        SequencePosition++;
        if (SequencePosition == 2)
        {
          // Restart signal
          I2C0_C1 |=  I2C_C1_RSTA_MASK;

          // Send slave address with read bit
          I2C0_D = I2CReadOrder[SequencePosition];
        }
        else if (SequencePosition == 3)
        {
          // Receive mode
          I2C0_C1 &= ~I2C_C1_TX_MASK;

          // Turn on ACK from master
          I2C0_C1 &= ~I2C_C1_TXAK_MASK;

          //Read data reg through pointer
          DataPtr[DataCounter] = I2C0_D;
        }
        else
        {
          // Send slave register address
          I2C0_D = I2CReadOrder[SequencePosition];
        }
        return;
      }
      //No ACK was recieved
      else
      {
        Stop();
        return;
      }
    }
    //in recieve mode
    else if (!(I2C0_C1 & I2C_C1_TX_MASK) && (DataCounter <NumOfBytes))
    {
      // Check if last byte to be read
      if (DataCounter == (NumOfBytes -1))
      {
        Stop();
        // Read last byte
        DataPtr[DataCounter] = I2C0_D;
        // Reset data counter
        DataCounter = 0;
        // Reset position counter
        SequencePosition = 0;
        // I2C interrupt disable
        I2C0_C1 &= ~I2C_C1_IICIE_MASK;
        // Clear interrupt flag
        I2C0_S |= I2C_S_IICIF_MASK;

        // Accelerometer read complete callback function
        (*ReadCompleteCallbackFunction)(ReadCompleteCallbackArguments);
        return;
      }
      // Check if second last byte to be read
      else if (DataCounter == (NumOfBytes -2))
      {
        // NACK from master
        I2C0_C1 |= I2C_C1_TXAK_MASK;
      }
      // Read data
      DataPtr[DataCounter] = I2C0_D;
      // Increment data counter
      DataCounter++;
    }
  }
}


/*!
 ** @}
 */