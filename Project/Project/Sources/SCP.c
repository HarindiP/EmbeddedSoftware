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
#include "Flash.h"
#include "packet.h"
#include "RTC.h"
#include "PE_Types.h"
#include "OS.h"

/*Tower Number*/
uint16union_t SCP_TowerNb;
/*Tower Mode*/
uint16union_t SCP_TowerMd;
/*Protocol Mode*/
uint8_t SCP_ProtocolMode;
/*Timing Mode*/
TRegulationMode SCP_RegMode;
/*Nb of lowers and raises*/
uint8_t SCP_Lowers;
uint8_t SCP_Raises;
//Vrms array
float SCP_Vrms[3] = {0,0,0};

/*Communication functions : */

bool SCP_SendStartUpValues()
{
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
  OS_DisableInterrupts();
  SCP_TowerNb.s.Lo = Packet_Parameter2;	//LSB
  SCP_TowerNb.s.Hi = Packet_Parameter3;	//MSB
  Flash_Write16((uint16_t *)NvTowerNb,SCP_TowerNb.l);	//writing in the flash memory
  OS_EnableInterrupts();
//  return Packet_Put(0x0B,1,NvTowerNb->s.Lo, NvTowerNb->s.Hi);	//Send Tower Number
  return SendTowerNumber();
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
  OS_DisableInterrupts();
  SCP_TowerMd.s.Lo = Packet_Parameter2;	//LSB : 1 if synchronous, 0 if asynchronous
  SCP_TowerMd.s.Hi = Packet_Parameter3;	//MSB : supposed to be 0
  Flash_Write16((uint16_t *)NvTowerMd,SCP_TowerMd.l);	//writing in the flash memory
  OS_EnableInterrupts();
//  return Packet_Put(0x0D,1, NvTowerMd->s.Lo, NvTowerMd->s.Hi);	//Send Tower Mode
  return SendTowerMode();
}


bool ReadByte(uint8_t address)
{
  if(address>=0x00 && address<=0x07)
    return Packet_Put(0x08,address,0,_FB(&address));
  return false;
}


bool ProgramByte(uint8_t address, uint8_t data)
{
  if(address == 0x08)
  {
    Flash_Erase();
    return true;
  }
  else
  {
      return Flash_Write8(&address, data);	//TODO : makes sense the & ?
  }
}


bool SendTime()
{
  RTC_Get(&Packet_Parameter1,&Packet_Parameter2,&Packet_Parameter3);
  return Packet_Put(0x0C,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
}
bool SetTime()
{
  RTC_Set(Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
  return SendTime();
}

bool SendTimingMode()
{
  return Packet_Put(0x10,(uint8_t)SCP_RegMode,0,0);
}

bool SetTimingMode()
{
  SCP_RegMode = Packet_Parameter1 - 1;
  Flash_Write8((uint8_t *)NvRegMode, SCP_RegMode);
  return SendTimingMode();
}
bool HandleTimingMode()
{
  if (Packet_Parameter1 == 0)
  {
    return SendTimingMode();
  }
  else if (Packet_Parameter1 == 1 || Packet_Parameter1 == 2)
  {
    return SetTimingMode();
  }
}

bool SendNbLowers()
{
  Flash_Write8((uint8_t *)NvNbLowers, SCP_Lowers);
  return Packet_Put(0x11,SCP_Lowers,0,0);
}
bool ResetLowers()
{
  SCP_Lowers = 0;
  Flash_Write8((uint8_t *)NvNbLowers, SCP_Lowers);
  return SendNbLowers();
}
bool HandleLowers()
{
  if (Packet_Parameter1 == 0)
  {
    return SendNbLowers();
  }
  else if (Packet_Parameter1 == 1)
  {
    return ResetLowers();
  }
}

bool SendNbRaises()
{
  Flash_Write8((uint8_t *)NvNbRaises, SCP_Raises);
  return Packet_Put(0x12,SCP_Raises,0,0);
}
bool ResetRaises()
{
  SCP_Raises = 0;
  Flash_Write8((uint8_t *)NvNbRaises, SCP_Raises);
  return SendNbRaises();
}
bool HandleRaises()
{
  if (Packet_Parameter1 == 0)
  {
    return SendNbRaises();
  }
  else if (Packet_Parameter1 == 1)
  {
    return ResetRaises();
  }
}

bool GetVrms()
{
  uint8_t vrms = SCP_Vrms[Packet_Parameter1] * 100;
  return Packet_Put(0x18,Packet_Parameter1,vrms / 100,vrms % 100); //TODO : OK ?
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
    case Timing_Mode :
      return HandleTimingMode();
      break;
    case Number_Of_Lowers :
      return HandleLowers();
      break;
    case Number_Of_Raises :
      return HandleRaises();
      break;
    case Voltage :
      return GetVrms();
      break;
    default:	//Unknown command
      //Do nothing or return command with NAK
      //Packet_Put(0b1000000 | Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
      break;
  }
}

bool SCP_Packet_Handle_Ack()
{
  Packet_Command &= ~PACKET_ACK_MASK;
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
