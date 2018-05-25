/*! @file Flash.h
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

#ifndef FLASH_H
#define FLASH_H

// new types
#include "types.h"


// FLASH data access
#define _FB(flashAddress)  *(uint8_t  volatile *)(flashAddress)
#define _FH(flashAddress)  *(uint16_t volatile *)(flashAddress)
#define _FW(flashAddress)  *(uint32_t volatile *)(flashAddress)
#define _FP(flashAddress)  *(uint64_t volatile *)(flashAddress)

// Address of the start of the Flash block we are using for data storage
#define FLASH_DATA_START 0x00080000LU
// Address of the end of the Flash block we are using for data storage
#define FLASH_DATA_END   0x00080007LU

// Private Function Dec's`
static bool LaunchCommand(TFCCOB* commonCommandObject);
static bool WritePhrase(const uint32_t address, const uint64union_t phrase);
static bool EraseSector(const uint32_t address);
static bool ModifyPhrase(const uint32_t address, const uint64union_t phrase);

/*! @brief Enables the Flash module.
 *
 *  @return bool - TRUE if the Flash was setup successfully.
 */
bool Flash_Init(void);
 
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
bool Flash_AllocateVar(volatile void** variable, const uint8_t size);
/*! @brief determine the next available location in the flash memory
 *
 * @param  size is the size of the data we are trying to store
 *
 * @return bool - TRUE if a place was found
 * */
bool SpaceAvailble(uint8_t checkAddress, uint8_t chosenAddress,const uint8_t size);

/*! @brief Writes a 32-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 32-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if address is not aligned to a 4-byte boundary or if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_Write32(volatile uint32_t* const address, const uint32_t data);
 
/*! @brief Writes a 16-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 16-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if address is not aligned to a 2-byte boundary or if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_Write16(volatile uint16_t* const address, const uint16_t data);

/*! @brief Writes an 8-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 8-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_Write8(volatile uint8_t* const address, const uint8_t data);

/*! @brief Erases the entire Flash sector.
 *
 *  @return bool - TRUE if the Flash "data" sector was erased successfully.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_Erase(void);

#endif

/* Notes a moi meme :
 * we have chosen to use Flash Block2, divided in 4 sectors
 * adresses go from 0x0008_0000 to 0x000B_FFFF
 *
 * must erase before writing : "This means that if we want to change just 1 byte in a 4 KiB sector, the entire
sector contents must be read into a RAM buffer, and the 1 byte changed in the
RAM buffer. Then the Flash sector must be erased, turning all 0�s into 1�s.
Finally, we must write the entire new 4 KiB sector from the RAM buffer to the
Flash sector."
 * thesmallest unit of data we can write to the Flash array is 8 bytes, which is called a
phrase. The phrase must be aligned on an 8-byte address, i.e. 0x0008_0000
 *
 * only operating in the first �phrase� of
sector 0 of Flash Block2 : 0x0008_0000 to 0x0008_0007
 * we can only program and read bytes with an offset between 0 and 7.
 *
 *
 * */

/*!
 ** @}
 */