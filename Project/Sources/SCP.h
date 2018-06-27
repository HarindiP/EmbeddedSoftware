/*! @file SCP.h
 *
 *  @brief Serial Communication Protocol.
 *
 *  This contains the functions needed for the serial communication.
 *
 *  @author 12443166
 *  @date 2018-04-14
 */
/*!
 **  @addtogroup SCP_module SCP module documentation
 **  @{
 */
/* MODULE SCP */

#ifndef SOURCES_SCP_H_
#define SOURCES_SCP_H_

/*Tower Number*/
extern uint16union_t SCP_TowerNb;
/*Tower Mode*/
extern uint16union_t SCP_TowerMd;


//Flash address of Tower Number and Mode
volatile uint16union_t *NvTowerNb;
volatile uint16union_t *NvTowerMd;

//Create an enum that toggles between mode
typedef enum
{
  DEF_MODE,
  INV_MODE
} Tmode;

extern Tmode SCP_TimingMode;

//Communication commands : PC to Tower
/*Special - Get startup values
Parameter 1: 0
Parameter 2: 0
Parameter 3: 0
Note: The PC will issue this command upon startup to retrieve the state of the Tower to update the interface application.*/
#define Get_Start_Up_Values 0x04	//Command is : Special - Get Start up Values
/*Special – Get version
Parameter 1: ‘v’
Parameter 2: ‘x’
Parameter 3: CR*/
#define Get_Version 0x09		//Command is : Special – Get version
/*Tower Number
Parameter 1: 1 = get Tower number
	     2 = set Tower number
Parameter 2: LSB for a “set”, 0 for a “get”
Parameter 3: MSB for a “set”, 0 for a “get”
Note: The Tower number is an unsigned 16-bit number*/
#define Tower_Number 0x0B		//Command is : Tower number (get & set)
/*Tower Mode
Parameter 1: 1 = get Tower mode
	     2 = set Tower mode
Parameter 2: LSB for a “set”, 0 for a “get”
Parameter 3: MSB for a “set”, 0 for a “get”
Note: The Tower mode is just an unsigned 16-bit number – it is
usually declared as an enumerated type.*/
#define Tower_Mode 0x0D			//Command is : Tower Mode (get & set)

/*Flash – Read byte
Parameter 1: address offset (0-7)
Parameter 2: 0
Parameter 3: 0
Note: The address offset has a range of 0x00 to 0x07.*/
#define Flash_Read_Byte 0x08		//Command is : Flash - Read byte

/*Flash – Program byte
Parameter 1: address offset (0-8)
Parameter 2: 0
Parameter 3: data
Note: The address offset has a range of 0x00 to 0x08.
An address offset of 0x08 will ERASE the entire Flash sector.*/
#define Flash_Program_Byte 0x07		//Command is : Flash - Program byte

/*Time
Parameter 1: hours (0-23)
Parameter 2: minutes (0-59)
Parameter 3: seconds (0-59)*/
#define Time 0x0C

/*Protocol – Mode
Parameter 1:  1 = get Protocol mode
              2 = set Protocol mode
Parameter 2:  0 = asynchronous for a “set”, 0 for a “get”
              1 = synchronous for a “set”, 0 for a “get”
Parameter 3: 0 */
#define Protocol_Mode 0x0A

/*Timing – Mode
Parameter 1:  0 = get Timing mode
              1 = set definite Mode (5 seconds)
              2 = set inverse Mode
Parameter 2:  0 = definite mode
              0 = inverse mode
Parameter 3: 0 */
#define Timing_Mode 0x10


#define Number_of_Raises 0x11

#define Number_of_Lowers 0x12

#define Frequency 0x17

#define Voltage 0x18

#define Spectrum 0x19


//Communication functions :

/*! @brief Send the start up values when tower is initialized
 *
 *  @return true if values has been sent, false if not
 */
bool SCP_SendStartUpValues();

/*! @brief Send Tower version
 *
 *  @return true if value has been sent, false if not
 */
bool SendVersion();

/*! @brief Identify the Tower Number command : set or just send it
 *
 *  @return true if values has been sent/set, false if not
 */
bool HandleTowerNumber();
/*! @brief Send the Tower Number
 *
 *  @return true if number has been sent, false if not
 */
bool SendTowerNumber();
/*! @brief Set the new Tower Number and send it
 *
 *  @return true if number has been set and sent, false if not
 */
bool SetTowerNumber();

/*! @brief Identify the Tower Mode command : set or just send it
 *
 *  @return true if values has been sent/set, false if not
 */
bool HandleTowerMode();
/*! @brief Send the Tower Mode
 *
 *  @return true if mode has been sent, false if not
 */
bool SendTowerMode();
/*! @brief Set the new Tower Mode and send it
 *
 *  @return true if mode has been set and sent, false if not
 */
bool SetTowerMode();

/*! @brief Read the byte in the flash memory at the given address
 *
 *  @param   address : The address were the desired byte
 *  @return   true if the byte has been sent, false if not
 */
bool ReadByte(uint8_t address);
/*! @brief Write a byte in the flash memory at the given address
 *
 *  @param   address : The address were the desired byte is written
 *  @param   data : The byte to write
 *  @return   true if the byte has been written, false if not
 */
bool ProgramByte(uint8_t address, uint8_t data);
/*! @brief Identify the Timing mode command :  set or just send it
 *
 *  @param
 *  @return   true if the values have been sent, false if not
 */
bool HandleTimingMode();

/*! @brief Send the timing mode
 *
 *  @param
 *  @return   true if the mode has been sent, false if not
 */
bool SendTimingMode();

/*! @brief Set the timing mode
 *
 *  @param
 *  @return   true if the mode has been sent, false if not
 */
bool SetTimingMode();

/*! @brief Identify the Highers command :  reset or just send it
 *
 *  @param
 *  @return   true if the values have been set/sent, false if not
 */
bool HandleHighers();
/*! @brief Send the Nb of Highers occured
 *
 *  @param
 *  @return   true if the values has been sent, false if not
 */
bool SendHighers();
/*! @brief Reset the Nb of Highers occured
 *
 *  @param
 *  @return   true if the values has been sent, false if not
 */
bool ResetHigher();

/*! @brief Identify the Lowers command :  reset or just send it
 *
 *  @param
 *  @return   true if the values have been set/sent, false if not
 */
bool HandleLowers();
/*! @brief Send the Nb of Lowers occured
 *
 *  @param
 *  @return   true if the values has been sent, false if not
 */
bool SendLowers();
/*! @brief Set the Nb of Lowers occured
 *
 *  @param
 *  @return   true if the values has been sent, false if not
 */
bool ResetLowers();

/*! @brief Get the present frequency
 *
 *  @param
 *  @return   true if the frequency has been sent, false if not
 */
bool GetFreq();

/*! @brief Set the vrms value
 *
 *  @param
 *  @return   true if the values has been sent, false if not
 */
bool GetVrms();

/*! @brief Private ACK checking function
 *
 *  @return bool - TRUE if acknowledgement required
 */
bool SCP_Acknowledgement_Required(const uint8_t command);
/*! @brief Acknowledge that the command has been taken care of
 *
 *  @return true if acknowledgement went right
 */
bool ACK();
/*! @brief UnAcknowledge that the command has not been taken care of
 *
 *  @return true if unacknowledgement went right
 */
bool NAK();

/*! @brief Identify the command received and call the right function to do as told
 *
 *  @return true if the action went well
 */
bool SCP_Packet_Handle();
/*! @brief  Identify the command with an acknowledgement requirement received
 *          and call the right function to do as told and acknowledge
 *
 *  @return true if the action went well
 */
bool SCP_Packet_Handle_Ack();



#endif /* SOURCES_SCP_H_ */

/*!
 ** @}
 */
