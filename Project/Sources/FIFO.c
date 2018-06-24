/*
 * FIFO.c
 *
 *  Created on: 11 Apr 2018
 *      Author: 12443166
 */
/*!
 **  @addtogroup FIFO_module FIFO module documentation
 **  @{
 */
/* MODULE FIFO */


#include "FIFO.h"
//#include "PE_Types.h"

/*! @brief Initialize the FIFO before first use.
 *
 *  @param FIFO A pointer to the FIFO that needs initializing.
 *  @return void
 */



void FIFO_Init(TFIFO * const FIFO)
{
  /*No data in the buffer*/
    FIFO->Start=0;
    FIFO->End=0;
    FIFO->BufferAccess = OS_SemaphoreCreate(1);
    FIFO->ItemAvailable = OS_SemaphoreCreate(0);
    FIFO->SpaceAvailable = OS_SemaphoreCreate(FIFO_SIZE);
}

/*! @brief Put one character into the FIFO.
 *
 *  @param FIFO A pointer to a FIFO struct where data is to be stored.
 *  @param data A byte of data to store in the FIFO buffer.
 *  @return bool - TRUE if data is successfully stored in the FIFO.
 *  @note Assumes that FIFO_Init has been called.
 */
void FIFO_Put(TFIFO * const FIFO, const uint8_t data)
{

    /*If buffer is not full, put data in at the next available space and update Fifo parameters*/
    OS_SemaphoreWait(FIFO->SpaceAvailable, 0);

    OS_DisableInterrupts();
    FIFO->Buffer[FIFO->End]=data;
    FIFO->End++;
    /*Makes sure Fifo Buffer circular*/
    if (FIFO->End==FIFO_SIZE)
    {
	FIFO->End=0;
    }

    OS_EnableInterrupts();

    OS_SemaphoreSignal(FIFO->ItemAvailable);


}

/*! @brief Get one character from the FIFO.
 *
 *  @param FIFO A pointer to a FIFO struct with data to be retrieved.
 *  @param dataPtr A pointer to a memory location to place the retrieved byte.
 *  @return bool - TRUE if data is successfully retrieved from the FIFO.
 *  @note Assumes that FIFO_Init has been called.
 */
void FIFO_Get(TFIFO * const FIFO, uint8_t * const dataPtr)
{
    OS_SemaphoreWait(FIFO->ItemAvailable, 0);
//    OS_SemaphoreWait(FIFO->BufferAccess, 0);
    OS_DisableInterrupts();
    *dataPtr=FIFO->Buffer[FIFO->Start];
    FIFO->Start++;
    /*Makes sure Fifo Buffer circular*/
    if (FIFO->Start==FIFO_SIZE)
    {
      FIFO->Start=0;
    }
    OS_EnableInterrupts();
    OS_SemaphoreSignal(FIFO->SpaceAvailable); //Signal that FIFO has space
//    OS_SemaphoreSignal(FIFO->BufferAccess);
}


/*!
 ** @}
 */
