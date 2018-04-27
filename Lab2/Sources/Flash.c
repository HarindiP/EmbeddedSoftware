/*! @file Flash.c
 *
 *  @brief Routines for erasing and writing to the Flash.
 *
 *  This contains the functions needed for accessing the internal Flash.
 *
 *  @author Coralie&Harindi
 *  @date 2018-04-14
 */
/*!
 **  @addtogroup flash_module flash module documentation
 **  @{
 */
/* MODULE flash */


// new types
#include "types.h"
#include "Flash.h"
#include "MK70F12.h"

// FLASH data access
#define _FB(flashAddress)  *(uint8_t  volatile *)(flashAddress)		//access a byte
#define _FH(flashAddress)  *(uint16_t volatile *)(flashAddress)		//access a half-word (2bytes)
#define _FW(flashAddress)  *(uint32_t volatile *)(flashAddress)		//access a word (4bytes)
#define _FP(flashAddress)  *(uint64_t volatile *)(flashAddress)		//access a phrase (8bytes)

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
  FTFE_FCCOB0 = commonCommandObject-> fcmd;
  FTFE_FCCOB3 = commonCommandObject-> address_0_7;
  FTFE_FCCOB2 = commonCommandObject-> address_8_15;
  FTFE_FCCOB1 = commonCommandObject-> address_16_23;


  //big endian
  FTFE_FCCOB4 = commonCommandObject->cmd_data.dataByte[3];
  FTFE_FCCOB5 = commonCommandObject->cmd_data.dataByte[2];
  FTFE_FCCOB6 = commonCommandObject->cmd_data.dataByte[1];
  FTFE_FCCOB7 = commonCommandObject->cmd_data.dataByte[0];
  FTFE_FCCOB8 = commonCommandObject->cmd_data.dataByte[7];
  FTFE_FCCOB9 = commonCommandObject->cmd_data.dataByte[6];
  FTFE_FCCOBA = commonCommandObject->cmd_data.dataByte[5];
  FTFE_FCCOBB = commonCommandObject->cmd_data.dataByte[4];

  FTFE_FSTAT = FTFE_FSTAT_CCIF_MASK;
  while (FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK != 0) {
      //ADVANCED: FEED A WATCHDOG
  };

  /* checks error checks*/
  return (FTFE_FSTAT & FTFE_FSTAT_ACCERR_MASK | (FTFE_FSTAT & FTFE_FSTAT_FPVIOL_MASK) != 0);
}
static bool WritePhrase(const uint32_t address, const uint64union_t phrase)
{
  /*create variable, put right values into right regs and then execute right command : program here*/
  TFCCOB writephrase;
  writephrase.address_0_7 = address;
  writephrase.address_8_15 = address >> 8;
  writephrase.address_16_23 = address >> 16 ;


  //how to write a unit64union to unint8_t[8] -ask danon :)
  writephrase.cmd_data.data = phrase.l;
  writephrase.fcmd = FLASH_PROGRAM_PHRASE;
  return (LaunchCommand(&writephrase));

}
static bool EraseSector(const uint32_t address)
{
  /*create variable, put right values into right regs and then execute right command : erase here*/
  TFCCOB erasesector;
  erasesector.address_0_7 = address;
  erasesector.address_8_15 = address >> 8;
  erasesector.address_16_23 = address >> 16 ;
  erasesector.fcmd = FLASH_ERASE_SECTOR;
  return (LaunchCommand(&erasesector));

}
static bool ModifyPhrase(const uint32_t address, const uint64union_t phrase)
{
  //Checks if EraseSector function is successful in the right location
  if (EraseSector(FLASH_DATA_START))
      return WritePhrase(address,phrase);
  return false;
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
  static uint8_t flashMemoryMap = 0;
  if(size == 1 || size == 2 || size == 4)
  {
      uint8_t chosenAddress = 1;	//0b00000001
      uint8_t checkAddress = chosenAddress;
      for (int i = 0; i < size ; i++)
	{
	  checkAddress |=checkAddress<<1;
	}
      while(chosenAddress!=0)
	{
	  if(!(flashMemoryMap & chosenAddress))		//if there is a space big enough there, done !
	    {
	      *variable = (volatile void**)(FLASH_DATA_START + chosenAddress);
	      flashMemoryMap |= chosenAddress;	//store the chose address as taken
	      return true;
	    }
	  else		//otherwise check the next area of the adequate size
	    {
	      chosenAddress = chosenAddress<<size;
	    }

	}
  }
  return false;
}

/*! @brief determine the next available location in the flash memory
 *
 * @param  size is the size of the data we are trying to store
 *
 * @return bool - TRUE if a place was found
 * */
/*bool SpaceAvailble(uint8_t checkAddress, uint8_t chosenAddress,const uint8_t size)
{
  if(chosenAddress != 0x00)				// if no address allocated, take the first one
    {
      checkAddress += 8*size;					// else take the next byte
      while(chosenAddress && checkAddress)
	{
	  checkAddress = (checkAddress << size);
	  if (checkAddress == 0x00)			// we have check all the memory and no space is available
	    {
	      return false;
	    }
	}
    }
  return true;
}*/

/*! @brief Writes a 32-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 32-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if address is not aligned to a 4-byte boundary or if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_Write32(volatile uint32_t* const address, const uint32_t data)
{
  uint32_t flashAddress = (uint32_t)address;
  if(flashAddress<FLASH_DATA_START || flashAddress>FLASH_DATA_END-3)
  {
    return false;
  }
  else
  {
    uint32_t pAddress;
    uint64union_t phrase;
    if((int)address % 4 == 0)	//if address divisible by 4
    {
      phrase.s.Hi = data;
      phrase.s.Lo = _FW(flashAddress+4);
      pAddress = flashAddress;
      return ModifyPhrase(pAddress, phrase);
    }
    else
    {
      return false;
    }
    }
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
  uint32_t flashAddress = (uint32_t)address;
  if(flashAddress<FLASH_DATA_START || flashAddress>FLASH_DATA_END-1)
  {
    return false;
  }
  else
  {
    uint32_t wAddress;
    uint32union_t word;
    if((int)address % 2 == 0)	//if even address
    {
      word.s.Hi = data;
      word.s.Lo = _FH(flashAddress+2);
      wAddress = flashAddress;
    }
    else			//address is odd : we make it even taking the byte before and the one after which give us a halfword
    {
      return false;
    }
    return Flash_Write32((uint32_t*)wAddress, word.l);
  }
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
  //Find the address of the halfWord that contains the byte we want to change
  // halfWord.l = _FH(address);
  //modify byte we want to change
  //...
  //halfWord.s.Lo = newData;
  //return Flash_Write16(address, halfWord.l);
  uint32_t flashAddress = (uint32_t)address;
  if(flashAddress<FLASH_DATA_START || flashAddress>FLASH_DATA_END)
    {
      return false;
    }
  else
    {
      uint32_t hwAddress;
      uint16union_t halfWord;
      if (flashAddress % 2 == 0)	//if even address
      {
	halfWord.s.Lo = data;
	halfWord.s.Hi = _FB(flashAddress + 1);
	hwAddress = flashAddress;
      }
      else
      {
	halfWord.s.Lo = _FB(flashAddress - 1);
	halfWord.s.Hi = data;
	hwAddress = flashAddress - 1;
      }
      return Flash_Write16((uint16_t *)hwAddress, halfWord.l);
    }
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


/*!
 ** @}
 */

