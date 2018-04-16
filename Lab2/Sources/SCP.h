/*
 * SCP.h
 *
 *  Created on: 14 avr. 2018
 *      Author: Coralie
 */

#ifndef SOURCES_SCP_H_
#define SOURCES_SCP_H_

/*Tower Number*/
extern uint16union_t towerNb;
/*Tower Mode*/
extern uint16union_t towerMd;

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

//Communication functions :
/*Tower Startup
Parameter 1: 0
Parameter 2: 0
Parameter 3: 0*/
bool SendStartUpValues();
/*Special – Tower version
Parameter 1: ‘v’ = version
Parameter 2: Major Version Number
Parameter 3: Minor Version Number (out of 100)*/
bool SendVersion();
/*Tower Number
Parameter 1: 1
Parameter 2: LSB
Parameter 3: MSB*/
bool SendTowerNumber(uint16union_t towerNb);

bool SetTowerNumber(uint16union_t towerNb);
/*Tower Mode
Parameter 1: 1
Parameter 2: LSB
Parameter 3: MSB*/
bool SendTowerMode(uint16union_t towerMd);

bool SetTowerMode(uint16union_t towerMd);
/*Flash – Read byte
Parameter 1: address offset (0-7)
Parameter 2: 0
Parameter 3: data*/
bool ReadByte(uint8_t address);

bool ProgramByte(uint8_t address, uint8_t data);

/*Acknowledgement and NonAcknowledgement functions*/
bool Packet_ACK();
bool Packet_NAK();



#endif /* SOURCES_SCP_H_ */
