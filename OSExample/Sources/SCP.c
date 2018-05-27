/*! @file SCP.c
 *
 *  @brief Serial Communication Protocol.
 *
 *  This contains the functions needed for the serial communication.
 *
 *  @author Coralie&Harindi
 *  @date 2018-04-14
 */
/*!
 **  @addtogroup SCP_module SCP module documentation
 **  @{
 */
/* MODULE SCP */

#include "types.h"
#include "SCP.h"
//#include "Flash.h"
#include "packet.h"
#include "RTC.h"
#include "PE_Types.h"
//#include "accel.h"

/*Tower Number*/
uint16union_t SCP_TowerNb;
/*Tower Mode*/
uint16union_t SCP_TowerMd;
/*Protocol Mode*/
uint8_t SCP_ProtocolMode;

/*Communication functions : */

bool SCP_SendStartUpValues()
{
  /*return (Packet_Put(0x04,0,0,0)			//Send Tower Start up
      &&Packet_Put(0x09,'v',1,0)			//Send Tower Version V1.0
      &&Packet_Put(0x0B,1,towerNb.s.Lo,towerNb.s.Hi)	//Send Tower Number
      &&Packet_Put(0x0D,1,towerMd.s.Lo,towerMd.s.Hi));	//Send Tower Mode*/
  return (Packet_Put(0x04,0,0,0) && SendVersion()  && SendTowerNumber() && SendTowerMode() /*&& SendProtocolMode()*/);
}


bool SendVersion()
{
  return Packet_Put(0x09,'v',1,0);			//Send Tower Version V1.0
}


bool HandleTowerNumber()
{
  if(Packet_Parameter1 == 1)		// Command is : get Tower Nb
  {
    SendTowerNumber();
  }
  else if (Packet_Parameter2==2)	//Command is : set Tower Number
  {
    SetTowerNumber();
  }
}
bool SendTowerNumber()
{
  return Packet_Put(0x08, 1, SCP_TowerNb.s.Lo, SCP_TowerNb.s.Hi);	//Send Tower Number
}
bool SetTowerNumber()
{
//  EnterCritical();
//  SCP_TowerNb.s.Lo = Packet_Parameter2;	//LSB
//  SCP_TowerNb.s.Hi = Packet_Parameter3;	//MSB
//  Flash_Write16((uint16_t *)NvTowerNb,SCP_TowerNb.l);	//writing in the flash memory
//  ExitCritical();
//  return Packet_Put(0x0B,1,NvTowerNb->s.Lo, NvTowerNb->s.Hi);	//Send Tower Number
}


bool HandleTowerMode()
{
  if(Packet_Parameter1 == 1)		// Command is : get Tower Mode
  {
    SendTowerMode();
  }
  else if (Packet_Parameter2==2)	//Command is : set Tower Mode
  {
    SetTowerMode();
  }
}
bool SendTowerMode()
{
  return Packet_Put(0x0D, 1, SCP_TowerMd.s.Lo, SCP_TowerMd.s.Hi);	//Send Tower Mode
}
bool SetTowerMode()
{
//  EnterCritical();
//  SCP_TowerMd.s.Lo = Packet_Parameter2;	//LSB : 1 if synchronous, 0 if asynchronous
//  SCP_TowerMd.s.Hi = Packet_Parameter3;	//MSB : supposed to be 0
//  Flash_Write16((uint16_t *)NvTowerMd,SCP_TowerMd.l);	//writing in the flash memory
//  ExitCritical();
//  return Packet_Put(0x0D,1, NvTowerMd->s.Lo, NvTowerMd->s.Hi);	//Send Tower Mode
}


bool ReadByte(uint8_t address)
{
  if(address>=0x00 && address<=0x07)
//    return Packet_Put(0x08,address,0,_FB(&address));
  return false;
}


bool ProgramByte(uint8_t address, uint8_t data)
{
  //Flash_AllocateVar(volatile void** variable, const uint8_t size)
//  if(address == 0x08)
//  {
//    Flash_Erase();
//    return true;
//  }
//  else
//  {
////    return Flash_Write8(&address, data);
//  }
}


bool SendTime()
{
  RTC_Get(&Packet_Parameter1,&Packet_Parameter2,&Packet_Parameter3);
  return Packet_Put(0x0C,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
}
bool SetTime()
{
//  RTC_Set(Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
  return SendTime();
}


bool SendProtocolMode()
{
  return Packet_Put(0x0A,1, SCP_ProtocolMode, 0);
}
bool SetProtocolMode()
{
  SCP_ProtocolMode = Packet_Parameter2;
//  Accel_SetMode(SCP_ProtocolMode);
  return SendProtocolMode();
}
bool HandleProtocolMode()
{
  if(Packet_Parameter1 == 1)    // Command is : get Tower Mode
  {
    SendProtocolMode();
  }
  else if (Packet_Parameter2 == 2)  //Command is : set Tower Mode
  {
    SetProtocolMode();
  }
}


bool SendAccelValues()
{
  uint8_t data[3];
//  Accel_ReadXYZ(data);
  return Packet_Put(0x10, data[0], data[1], data[2]);
}


/*Acknowledgement and NonAcknowledgement functions*/
bool SCP_Acknowledgement_Required(const uint8_t command)
{
  return (command & PACKET_ACK_MASK);
}
bool ACK()
{
  return Packet_Put(Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
}
bool NAK()
{
  return Packet_Put(PACKET_ACK_MASK|Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
}

bool SCP_Packet_Handle()
{
  switch (Packet_Command)
  {
    case Get_Start_Up_Values :		//Command is : Special - Get Start up Values
      return SCP_SendStartUpValues();
      break;
    case Get_Version :		//Command is : Special  Get version
      return SendVersion();
      break;
    case Tower_Number :		//Command is : Tower number (get & set)
      return HandleTowerNumber();
      break;
    case Tower_Mode :		//Command is : Tower Mode (get & set)
      return HandleTowerMode();
      break;
    case Flash_Read_Byte :
      return ReadByte(Packet_Parameter1);
      break;
    case Flash_Program_Byte :
      return ProgramByte(Packet_Parameter1,Packet_Parameter3);
      break;
    case Time :
      return SetTime();
      break;
    case Protocol_Mode :
      return HandleProtocolMode();
      break;
    case Accel_Value :
      return SendAccelValues();
      break;
    default:	//Unknown command
      //Do nothing or return command with NAK
      //Packet_Put(0b1000000 | Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
      break;
  }
}

bool SCP_Packet_Handle_Ack()
{
  Packet_Command |= PACKET_ACK_MASK;
  if(SCP_Packet_Handle())
  {
    ACK();
  }
  else
  {
    NAK();
  }
}

/*!
 ** @}
 */
