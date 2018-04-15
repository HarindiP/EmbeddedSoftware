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
#define FLASH_PROGRAM_PHRASE 0X07
// the command that erases a flash block
#define FLASH_ERASE_SECTOR 0X09

// Private Function Dec's
static bool LaunchCommand(TFCCOB* commonCommandObject)
{
  /*Set ACCERR bit and FPVIOL bit to zero*/
  FTFE_FSTAT = FTFE_FSTAT_ACCERR_MASK | FTFE_FSTAT_FPVIOL_MASK;

  /*FTFE_FSTAT_CCIF_MASK =1  CLEARS IT
   * FTFE_FSTAT_CCIF_MASK = 0 SETS IT that launches it*/
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

  while (FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK != 0){}

  /* checks error checks*/
  return (FTFE_FSTAT & FTFE_FSTAT_ACCERR_MASK | (FTFE_FSTAT & FTFE_FSTAT_FPVIOL_MASK) != 0);


}
static bool WritePhrase(const uint32_t address, const unint64union_t phrase)
{
  /*create variable but right values into right regs and then execute righ command*/
  TFCCOB writephrase;
  writephrase.addressreg.address_t = address;
  writephrase.datacmd.data = phrase.l;
  writephrase.fcmd = FTFE_FCCOB0_CCOBn(FLASH_PROGRAM_PHRASE);
  return (LaunchCommand(&writephrase));

}
static bool EraseSector(const uint32_t address)
{
  /**/
  TFCCOB erasesector;
  erasesector.addressreg.address_t = address;
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
  /*user allocates  memory we create pointer to said variable */
  /*if variable is a byte any adress
   * if variable is a 2 byte= half word -> only even address
   * if variable is a 4 byte=word*/


  /*switch 3 case regarded*/
  switch(size){
    case1:
    case2:
    case3:
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
  // callFlash_Write32
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
  // call Flash_Write16
}

/*! @brief Erases the entire Flash sector.
 *
 *  @return bool - TRUE if the Flash "data" sector was erased successfully.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_Erase(void)
{

}







