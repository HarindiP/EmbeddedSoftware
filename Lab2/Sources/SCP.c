/*
 * SCP.c
 *
 *  Created on: 14 avr. 2018
 *      Author: Coralie
 */

#include "types.h"
#include "SCP.h"

/*Communication functions : */
/*Tower Startup
Parameter 1: 0
Parameter 2: 0
Parameter 3: 0*/
bool SendStartUpValues()
{
  return (Packet_Put(0x04,0,0,0)			//Send Tower Start up
      &&Packet_Put(0x09,'v',1,0)			//Send Tower Version V1.0
      &&Packet_Put(0x0B,1,towerNb.s.Lo,towerNb.s.Hi)	//Send Tower Number
      &&Packet_Put(0x0D,1,towerMd.s.Lo,towerMd.s.Hi));	//Send Tower Mode
}
/*Special � Tower version
Parameter 1: �v� = version
Parameter 2: Major Version Number
Parameter 3: Minor Version Number (out of 100)*/
bool SendVersion()
{
  return Packet_Put(0x09,'v',1,0);			//Send Tower Version V1.0
}
/*Tower Number
Parameter 1: 1
Parameter 2: LSB
Parameter 3: MSB*/
bool SendTowerNumber()
{
  return Packet_Put(0x0B,1,towerNb.s.Lo,towerNb.s.Hi);	//Send Tower Number
}
bool SetTowerNumber()
{
  towerNb.s.Lo = Packet_Parameter2;	//LSB
  towerNb.s.Hi = Packet_Parameter3;	//MSB
  return Packet_Put(0x0B,1,towerNb.s.Lo,towerNb.s.Hi);	//Send Tower Number
}

/*Tower Mode
Parameter 1: 1
Parameter 2: LSB
Parameter 3: MSB*/
bool SendTowerMode()
{
  return Packet_Put(0x0D,1,towerMd.s.Lo,towerMd.s.Hi);	//Send Tower Mode
}
bool SetTowerMode()
{
  towerMd.s.Lo = Packet_Parameter2;	//LSB : 1 if synchronous, 0 if asynchronous
  towerMd.s.Hi = Packet_Parameter3;	//MSB : supposed to be 0
  return Packet_Put(0x0D,1,towerMd.s.Lo,towerMd.s.Hi);	//Send Tower Mode
}

/*Flash � Read byte
Parameter 1: address offset (0-7)
Parameter 2: 0
Parameter 3: data*/
bool ReadByte()
{

}

bool ProgramByte()
{

}

/*Acknowledgement and NonAcknowledgement functions*/
bool Packet_ACK()
{
  return Packet_Put(Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
}
bool Packet_NAK()
{
  return Packet_Put(PACKET_ACK_MASK|Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
}

