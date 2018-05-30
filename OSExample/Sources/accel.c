/*! @file accel.c
 *
 *  @brief HAL for the accelerometer.
 *
 *  This contains the functions for interfacing to the MMA8451Q accelerometer.
 *
 *  @author Coralie&Harindi
 *  @date 2018-05-12
 */

/*!
 **  @addtogroup accel_module accel module documentation
 **  @{
 */
/* MODULE accel */

// Accelerometer functions
#include "accel.h"

// Inter-Integrated Circuit
#include "I2C.h"

// Median filter
#include "median.h"

// LEDs
#include "LEDs.h"

//SCP
#include "SCP.h"

// K70 module registers
#include "MK70F12.h"

// CPU and PE_types are needed for critical section variables and the defintion of NULL pointer
#include "CPU.h"
#include "PE_types.h"



// Accelerometer registers
#define ADDRESS_OUT_X_MSB 0x01
#define ADDRESS_OUT_X_LSB 0x02
#define ADDRESS_OUT_Y_MSB 0x03
#define ADDRESS_OUT_Y_LSB 0x04
#define ADDRESS_OUT_Z_MSB 0x05
#define ADDRESS_OUT_Z_LSB 0x06

#define ADDRESS_INT_SOURCE 0x0C

static union
{
  uint8_t byte;     /*!< The INT_SOURCE bits accessed as a byte. */
  struct
  {
    uint8_t SRC_DRDY   : 1; /*!< Data ready interrupt status. */
    uint8_t               : 1;
    uint8_t SRC_FF_MT  : 1; /*!< Freefall/motion interrupt status. */
    uint8_t SRC_PULSE  : 1; /*!< Pulse detection interrupt status. */
    uint8_t SRC_LNDPRT : 1; /*!< Orientation interrupt status. */
    uint8_t SRC_TRANS  : 1; /*!< Transient interrupt status. */
    uint8_t SRC_FIFO   : 1; /*!< FIFO interrupt status. */
    uint8_t SRC_ASLP   : 1; /*!< Auto-SLEEP/WAKE interrupt status. */
  } bits;     /*!< The INT_SOURCE bits accessed individually. */
} INT_SOURCE_Union;

#define INT_SOURCE        INT_SOURCE_Union.byte
#define INT_SOURCE_SRC_DRDY INT_SOURCE_Union.bits.SRC_DRDY
#define INT_SOURCE_SRC_FF_MT  CTRL_REG4_Union.bits.SRC_FF_MT
#define INT_SOURCE_SRC_PULSE  CTRL_REG4_Union.bits.SRC_PULSE
#define INT_SOURCE_SRC_LNDPRT CTRL_REG4_Union.bits.SRC_LNDPRT
#define INT_SOURCE_SRC_TRANS  CTRL_REG4_Union.bits.SRC_TRANS
#define INT_SOURCE_SRC_FIFO CTRL_REG4_Union.bits.SRC_FIFO
#define INT_SOURCE_SRC_ASLP CTRL_REG4_Union.bits.SRC_ASLP

#define ADDRESS_CTRL_REG1 0x2A

typedef enum
{
  DATE_RATE_800_HZ = 0b000,
  DATE_RATE_400_HZ = 0b001,
  DATE_RATE_200_HZ = 0b010,
  DATE_RATE_100_HZ = 0b011,
  DATE_RATE_50_HZ = 0b100,
  DATE_RATE_12_5_HZ = 0b101,
  DATE_RATE_6_25_HZ = 0b110,
  DATE_RATE_1_56_HZ = 0b111
} TOutputDataRate;

typedef enum
{
  SLEEP_MODE_RATE_50_HZ = 0b00,
  SLEEP_MODE_RATE_12_5_HZ = 0b01,
  SLEEP_MODE_RATE_6_25_HZ = 0b10,
  SLEEP_MODE_RATE_1_56_HZ = 0b11
} TSLEEPModeRate;

static union
{
  uint8_t byte;     /*!< The CTRL_REG1 bits accessed as a byte. */
  struct
  {
    uint8_t ACTIVE    : 1;  /*!< Mode selection. */
    uint8_t F_READ    : 1;  /*!< Fast read mode. */
    uint8_t LNOISE    : 1;  /*!< Reduced noise mode. */
    uint8_t DR        : 3;  /*!< Data rate selection. */
    uint8_t ASLP_RATE : 2;  /*!< Auto-WAKE sample frequency. */
  } bits;     /*!< The CTRL_REG1 bits accessed individually. */
} CTRL_REG1_Union;

#define CTRL_REG1             CTRL_REG1_Union.byte
#define CTRL_REG1_ACTIVE      CTRL_REG1_Union.bits.ACTIVE
#define CTRL_REG1_F_READ      CTRL_REG1_Union.bits.F_READ
#define CTRL_REG1_LNOISE      CTRL_REG1_Union.bits.LNOISE
#define CTRL_REG1_DR          CTRL_REG1_Union.bits.DR
#define CTRL_REG1_ASLP_RATE   CTRL_REG1_Union.bits.ASLP_RATE

#define ADDRESS_CTRL_REG2 0x2B

#define ADDRESS_CTRL_REG3 0x2C

static union
{
  uint8_t byte;     /*!< The CTRL_REG3 bits accessed as a byte. */
  struct
  {
    uint8_t PP_OD       : 1;  /*!< Push-pull/open drain selection. */
    uint8_t IPOL        : 1;  /*!< Interrupt polarity. */
    uint8_t WAKE_FF_MT  : 1;  /*!< Freefall/motion function in SLEEP mode. */
    uint8_t WAKE_PULSE  : 1;  /*!< Pulse function in SLEEP mode. */
    uint8_t WAKE_LNDPRT : 1;  /*!< Orientation function in SLEEP mode. */
    uint8_t WAKE_TRANS  : 1;  /*!< Transient function in SLEEP mode. */
    uint8_t FIFO_GATE   : 1;  /*!< FIFO gate bypass. */
  } bits;     /*!< The CTRL_REG3 bits accessed individually. */
} CTRL_REG3_Union;

#define CTRL_REG3             CTRL_REG3_Union.byte
#define CTRL_REG3_PP_OD       CTRL_REG3_Union.bits.PP_OD
#define CTRL_REG3_IPOL        CTRL_REG3_Union.bits.IPOL
#define CTRL_REG3_WAKE_FF_MT  CTRL_REG3_Union.bits.WAKE_FF_MT
#define CTRL_REG3_WAKE_PULSE  CTRL_REG3_Union.bits.WAKE_PULSE
#define CTRL_REG3_WAKE_LNDPRT CTRL_REG3_Union.bits.WAKE_LNDPRT
#define CTRL_REG3_WAKE_TRANS  CTRL_REG3_Union.bits.WAKE_TRANS
#define CTRL_REG3_FIFO_GATE   CTRL_REG3_Union.bits.FIFO_GATE

#define ADDRESS_CTRL_REG4 0x2D

static union
{
  uint8_t byte;     /*!< The CTRL_REG4 bits accessed as a byte. */
  struct
  {
    uint8_t INT_EN_DRDY   : 1;  /*!< Data ready interrupt enable. */
    uint8_t               : 1;
    uint8_t INT_EN_FF_MT  : 1;  /*!< Freefall/motion interrupt enable. */
    uint8_t INT_EN_PULSE  : 1;  /*!< Pulse detection interrupt enable. */
    uint8_t INT_EN_LNDPRT : 1;  /*!< Orientation interrupt enable. */
    uint8_t INT_EN_TRANS  : 1;  /*!< Transient interrupt enable. */
    uint8_t INT_EN_FIFO   : 1;  /*!< FIFO interrupt enable. */
    uint8_t INT_EN_ASLP   : 1;  /*!< Auto-SLEEP/WAKE interrupt enable. */
  } bits;     /*!< The CTRL_REG4 bits accessed individually. */
} CTRL_REG4_Union;

#define CTRL_REG4               CTRL_REG4_Union.byte
#define CTRL_REG4_INT_EN_DRDY   CTRL_REG4_Union.bits.INT_EN_DRDY
#define CTRL_REG4_INT_EN_FF_MT  CTRL_REG4_Union.bits.INT_EN_FF_MT
#define CTRL_REG4_INT_EN_PULSE  CTRL_REG4_Union.bits.INT_EN_PULSE
#define CTRL_REG4_INT_EN_LNDPRT CTRL_REG4_Union.bits.INT_EN_LNDPRT
#define CTRL_REG4_INT_EN_TRANS  CTRL_REG4_Union.bits.INT_EN_TRANS
#define CTRL_REG4_INT_EN_FIFO   CTRL_REG4_Union.bits.INT_EN_FIFO
#define CTRL_REG4_INT_EN_ASLP   CTRL_REG4_Union.bits.INT_EN_ASLP

#define ADDRESS_CTRL_REG5 0x2E

static union
{
  uint8_t byte;     /*!< The CTRL_REG5 bits accessed as a byte. */
  struct
  {
    uint8_t INT_CFG_DRDY   : 1; /*!< Data ready interrupt enable. */
    uint8_t                : 1;
    uint8_t INT_CFG_FF_MT  : 1; /*!< Freefall/motion interrupt enable. */
    uint8_t INT_CFG_PULSE  : 1; /*!< Pulse detection interrupt enable. */
    uint8_t INT_CFG_LNDPRT : 1; /*!< Orientation interrupt enable. */
    uint8_t INT_CFG_TRANS  : 1; /*!< Transient interrupt enable. */
    uint8_t INT_CFG_FIFO   : 1; /*!< FIFO interrupt enable. */
    uint8_t INT_CFG_ASLP   : 1; /*!< Auto-SLEEP/WAKE interrupt enable. */
  } bits;     /*!< The CTRL_REG5 bits accessed individually. */
} CTRL_REG5_Union;

#define CTRL_REG5                 CTRL_REG5_Union.byte
#define CTRL_REG5_INT_CFG_DRDY    CTRL_REG5_Union.bits.INT_CFG_DRDY
#define CTRL_REG5_INT_CFG_FF_MT   CTRL_REG5_Union.bits.INT_CFG_FF_MT
#define CTRL_REG5_INT_CFG_PULSE   CTRL_REG5_Union.bits.INT_CFG_PULSE
#define CTRL_REG5_INT_CFG_LNDPRT  CTRL_REG5_Union.bits.INT_CFG_LNDPRT
#define CTRL_REG5_INT_CFG_TRANS   CTRL_REG5_Union.bits.INT_CFG_TRANS
#define CTRL_REG5_INT_CFG_FIFO    CTRL_REG5_Union.bits.INT_CFG_FIFO
#define CTRL_REG5_INT_CFG_ASLP    CTRL_REG5_Union.bits.INT_CFG_ASLP

#define ADDRESS_SLAVE 0x1D

void (*dataReadyCallbackFunction)(void*); /*!< The user's data ready callback function. */
void* dataReadyCallbackArguments;   /*!< The user's data ready callback function arguments. */

//Semaphore
OS_ECB* accelAccess;


//We should have a standBy and an other state function, and standBy is critical so thats the only place were we call PollRead()

bool Accel_Init(const TAccelSetup* const accelSetup)
{
  dataReadyCallbackFunction = accelSetup->dataReadyCallbackFunction;
  dataReadyCallbackArguments = accelSetup->dataReadyCallbackArguments;

  TI2CModule I2CModule;
  I2CModule.primarySlaveAddress = ADDRESS_SLAVE;
  I2CModule.readCompleteCallbackArguments = accelSetup->readCompleteCallbackFunction;
  I2CModule.readCompleteCallbackFunction = accelSetup->readCompleteCallbackArguments;
  I2CModule.baudRate = 100000;
  I2C_Init(&I2CModule, accelSetup->moduleClk);
  // select slave
  I2C_SelectSlaveDevice(ADDRESS_SLAVE);

//  //Test poll read with 1 data : temp should be 0x1A, 0x0D is Who Am I
//  uint8_t temp ;
//  I2C_PollRead(0x0D,&temp,1);

//  //Test poll read with 3 data : temp should be 0,0,great value (like 128) if we dont shake the tower;
//  uint8_t temp[3] = {0,0,0};
//  I2C_PollRead(ADDRESS_OUT_X_MSB,temp,3);

//    //Test int read with 3 data : temp should be 0,0,great value (like 128) if we dont shake the tower;
//    uint8_t temp[3] = {0,0,0};
//    I2C_IntRead(ADDRESS_OUT_X_MSB,temp,3);

  //Init PORTB 4 as Input
  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;  //clock
  PORTB_PCR4 = PORT_PCR_MUX(1);   //GPIO
  GPIOA_PDDR &= ~0x10;    //PORTB4 Input

  //Init in POLL mode
  Accel_SetMode(ACCEL_POLL);

  //Create Semaphore
  accelAccess = OS_SemaphoreCreate(0);

  // Clear any pending interrupts on PORTB4
  NVICICPR2 = NVIC_ICPR_CLRPEND(1 << 24);
  // Enable interrupts from PORTB4
  NVICISER2 = NVIC_ISER_SETENA(1 << 24);

  return true;
}

void Accel_ReadXYZ(uint8_t data[3])
{
  //array to store the 3 last data registered
  static uint8_t XData[3] = {0,0,0};
  static uint8_t YData[3] = {0,0,0};
  static uint8_t ZData[3] = {0,0,0};
  //index to erase the oldest data when full
  static uint8_t index = 0;

  //read
//  I2C_IntRead(ADDRESS_OUT_X_MSB, &XData[index],1);
//  I2C_IntRead(ADDRESS_OUT_Y_MSB, &YData[index],1);
//  I2C_IntRead(ADDRESS_OUT_Z_MSB, &ZData[index],1);
  I2C_PollRead(ADDRESS_OUT_X_MSB, &XData[index],3);
  //increase index
  index++;

  //Taking median data
  data[0] = Median_Filter3(XData[0],XData[1],XData[2]);
  data[1] = Median_Filter3(YData[0],YData[1],YData[2]);
  data[2] = Median_Filter3(ZData[0],ZData[1],ZData[2]);

  //wrap index
  if(index == 3)
  {
    index = 0;
  }

  //Toggle Green LED when read data
  LEDs_Toggle(LED_GREEN);
}

void Accel_SetMode(const TAccelMode mode)
{
  OS_DisableInterrupts();

  Standby(true);
  CTRL_REG4_INT_EN_DRDY = mode;
  CTRL_REG5_INT_CFG_DRDY = 1; // use INT1 pin

  CTRL_REG1_ACTIVE = 0; //desactivate accel before selecting mode
  CTRL_REG1_F_READ = 1; //only use 8 bit precision
  CTRL_REG1_LNOISE = 0; //set noise to zero to achieve high resolution
  CTRL_REG1_DR = DATE_RATE_1_56_HZ; //select data sending rate 1.56Hz
  I2C_Write(ADDRESS_CTRL_REG1, CTRL_REG1);
  I2C_Write(ADDRESS_CTRL_REG4, CTRL_REG4);
  I2C_Write(ADDRESS_CTRL_REG5, CTRL_REG5);

  CTRL_REG1_ACTIVE = 1; //re-activate accel
  I2C_Write(ADDRESS_CTRL_REG1, CTRL_REG1);

  if (mode == ACCEL_POLL)
  {
    PORTB_PCR4 &= ~PORT_PCR_IRQC_MASK;  // no interruption needed
  }
  else if (mode == ACCEL_INT)
  {
    PORTB_PCR4 |= PORT_PCR_ISF_MASK; //Clear pending flag
    //Set interrupt mode on 1001 : Flag and Interrupt on rising-edge.
    PORTB_PCR4 |= PORT_PCR_IRQC(9);
  }
  else;
  Standby(false);
  OS_EnableInterrupts();
}

static void Standby(bool SB)
{
  if (SB)
  {
    CTRL_REG1_ACTIVE = 0; //set standby
    I2C_Write(ADDRESS_CTRL_REG1, CTRL_REG1);
  }
  else
  {
    CTRL_REG1_ACTIVE = 1; //set active
    I2C_Write(ADDRESS_CTRL_REG1, CTRL_REG1);
  }
}

//

void __attribute__ ((interrupt)) AccelDataReady_ISR(void)
{
  OS_ISREnter();
  //Check interrupt enable
  if(PORTB_PCR4 & PORT_PCR_IRQC_MASK)
  {
    //Check flag
    if(PORTB_PCR4 & PORT_PCR_ISF_MASK)
    {
	I2C_PollRead(ADDRESS_INT_SOURCE) //INT_SOURCE_SRC_DRDY
      //Clear flag
      PORTB_PCR4 |= PORT_PCR_ISF_MASK;
      //Call callback function
//      if(dataReadyCallbackFunction)
//      {
//        dataReadyCallbackFunction(dataReadyCallbackArguments);
//      }
      OS_SemaphoreSignal(accelAccess);
    }
  }
  OS_ISRExit();
}

/*!
 * @}
*/
