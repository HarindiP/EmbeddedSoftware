/*
 * Flash.c
 *
 * @brief Routines for erasing and writing to the Flash.
 *
 *  This contains the functions needed for accessing the internal Flash.
 *
 *  Created on: 6 Apr 2018
 *      Author: 13115605
 */



// new types
#include "types.h"
#include "Flash.h"
#include "MK70F12.h"

// FLASH data access
#define _FB(flashAddress)  *(uint8_t  volatile *)(flashAddress)
#define _FH(flashAddress)  *(uint16_t volatile *)(flashAddress)
#define _FW(flashAddress)  *(uint32_t volatile *)(flashAddress)
#define _FP(flashAddress)  *(uint64_t volatile *)(flashAddress)

// Address of the start of the Flash block we are using for data storage
#define FLASH_DATA_START 0x00080000LU
// Address of the end of the Flash block we are using for data storage
#define FLASH_DATA_END   0x00080007LU
// The command that programs 8 bytes into flash
#define FLASH_PROGRAM_PHRASE 0x07
// the command that erases a flash block
#define FLASH_ERASE_SECTOR 0x09

// Private Function Dec's
static bool LaunchCommand(TFCCOB* commonCommandObject)		//chap 30 p806
{
  /*Set ACCERR bit and FPVIOL bit to zero*/
  FTFE_FSTAT = FTFE_FSTAT_ACCERR_MASK | FTFE_FSTAT_FPVIOL_MASK;

  /*FTFE_FSTAT_CCIF_MASK =1  CLEARS IT
   * FTFE_FSTAT_CCIF_MASK = 0 SETS IT that launches it*/
  do
  {
      FTFE_FCCOB3 = commonCommandObject-> addressreg.address_t.reg0;
      FTFE_FCCOB2 = commonCommandObject-> addressreg.address_t.reg1;
      FTFE_FCCOB1 = commonCommandObject-> addressreg.address_t.reg2;
      FTFE_FCCOB0 = commonCommandObject-> fcmd;

      FTFE_FCCOB4 = commonCommandObject->datacmd.databyte[0];
      FTFE_FCCOB5 = commonCommandObject->datacmd.databyte[1];
      FTFE_FCCOB6 = commonCommandObject->datacmd.databyte[2];
      FTFE_FCCOB7 = commonCommandObject->datacmd.databyte[3];
      FTFE_FCCOB8 = commonCommandObject->datacmd.databyte[4];
      FTFE_FCCOB9 = commonCommandObject->datacmd.databyte[5];
      FTFE_FCCOBA = commonCommandObject->datacmd.databyte[6];
      FTFE_FCCOBB = commonCommandObject->datacmd.databyte[7];

      FTFE_FSTAT = FTFE_FSTAT_CCIF_MASK;
    }while (FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK != 0);

  /* checks error checks*/
  return (FTFE_FSTAT & FTFE_FSTAT_ACCERR_MASK | (FTFE_FSTAT & FTFE_FSTAT_FPVIOL_MASK) != 0);


}
static bool WritePhrase(const uint32_t address, const uint64union_t phrase)
{
  /*create variable but right values into right regs and then execute right command*/
  TFCCOB writephrase;
  writephrase.addressreg.address = address;
  writephrase.datacmd.data = phrase.l;
  writephrase.fcmd = FTFE_FCCOB0_CCOBn(FLASH_PROGRAM_PHRASE);
  return (LaunchCommand(&writephrase));

}
static bool EraseSector(const uint32_t address)
{
  /**/
  TFCCOB erasesector;
  erasesector.addressreg.address = address;
  erasesector.fcmd = FTFE_FCCOB0_CCOBn(FLASH_ERASE_SECTOR);
  return (LaunchCommand(&erasesector));

}
static bool ModifyPhrase(const uint32_t address, const uint64union_t phrase)
{
  //Checks if EraseSector function is successful in the right location
  if ((!EraseSector(FLASH_DATA_START)))
    return false;
  return WritePhrase(address,phrase);
}

/*! @brief Enables the Flash module.
 *
 *  @return bool - TRUE if the Flash was setup successfully.
 */
bool Flash_Init(void)
{
  return true;
}

/*! @brief Allocates space for a non-volatile variable in the Flash memory.
 *
 *  @param variable is the address of a pointer to a variable that is to be allocated space in Flash memory.
 *         The pointer will be allocated to a relevant address:
 *         If the variable is a byte, then any address.
 *         If the variable is a half-word, then an even address.
 *         If the variable is a word, then an address divisible by 4.
 *         This allows the resulting variable to be used with the relevant Flash_Write function which assumes a certain memory address.
 *         e.g. a 16-bit variable will be on an even address
 *  @param size The size, in bytes, of the variable that is to be allocated space in the Flash memory. Valid values are 1, 2 and 4.
 *  @return bool - TRUE if the variable was allocated space in the Flash memory.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_AllocateVar(volatile void** variable, const uint8_t size)
{
  uint8_t chosenAddress = 0x00;
  uint8_t checkAddress = 0x00;

  switch(size){
    case 1:
      if(chosenAddress != 0x00){				// if no address allocated, take the first one
	  checkAddress++;					// else take the next one
	  while(chosenAddress && checkAddress){
	      checkAddress = (checkAddress << size);
	      if (checkAddress == 0x00){			// we have check all the memory and no space is available
		  return false;
		  break;
	      }
	  }
      }
      *variable = &_FW(FLASH_DATA_START + checkAddress);	// allocate variable at the right place
      chosenAddress ^= checkAddress;				// and store the chosen address
      return true;
      break;

    case 2:
      if(chosenAddress != 0x00){
	  checkAddress += 0x03;
	  while(chosenAddress && checkAddress){
	      checkAddress = (checkAddress << size);
	      if (checkAddress == 0x00){
		  return false;
		  break;
	      }
	  }
	  return true;}


      *variable = &_FW(FLASH_DATA_START + checkAddress);
      chosenAddress ^= checkAddress;
      return true;
      break;
    case 4:
      if(chosenAddress != 0x00){
	  checkAddress += 0x0F;
	  while(chosenAddress && checkAddress){
	      checkAddress = (checkAddress << size);
	      if (checkAddress == 0x00){
		  return false;
		  break;
	      }
	  }
      }


      *variable = &_FW(FLASH_DATA_START + checkAddress);
      chosenAddress ^= checkAddress;
      return true;
      break;

  }
}

/*! @brief Writes a 32-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 32-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if address is not aligned to a 4-byte boundary or if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_Write32(volatile uint32_t* const address, const uint32_t data)
{
  uint64union_t phrase;
  if(((uint32_t)address & 0b100) == 0)
    {
      phrase.s.Hi = data;
      phrase.s.Lo = (*(address+1) << 16) | *(address+2);
    }
  else
    {
      phrase.s.Lo = data;
      phrase.s.Hi = (*(address-2) << 16) | (*(address-1) << 8);
    }
  return ModifyPhrase(FLASH_DATA_START,phrase);
}

/*! @brief Writes a 16-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 16-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if address is not aligned to a 2-byte boundary or if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_Write16(volatile uint16_t* const address, const uint16_t data)
{
  uint32_t word;
  uint32_t wAddress = FLASH_DATA_START | (uint32_t)(address) & 0xFFFFFF00;
  if(((uint32_t)address & 0b10) == 0)
    {
      word = (((uint32_t)data) << 16) | *(address+1);
    }
  else
    {
      word = *(address-1) << 16 | (uint32_t)data;
    }
  return Flash_Write32((uint32_t *)wAddress,word);

}

/*! @brief Writes an 8-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 8-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_Write8(volatile uint8_t* const address, const uint8_t data)
{
  uint16_t halfWord;
  uint32_t hwAddress = ((uint32_t)(*address & 0xFE));
  if(((uint32_t)address & 1) == 0)
    {
      halfWord = (((uint16_t)data) << 8) | *(address+1);
    }
  else
    {
      halfWord = (*(address-1) << 8) | data;
    }
  return Flash_Write16((uint16_t *)hwAddress,halfWord);
}

/*! @brief Erases the entire Flash sector.
 *
 *  @return bool - TRUE if the Flash "data" sector was erased successfully.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_Erase(void)
{
  return EraseSector(FLASH_DATA_START);
}







