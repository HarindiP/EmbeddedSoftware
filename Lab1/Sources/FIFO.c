/*
 * FIFO.c
 *
 *  Created on: 28 Mar 2018
 *      Author: 13115605
 */


#include "FIFO.h"

/*! @brief Initialize the FIFO before first use.
 *
 *  @param FIFO A pointer to the FIFO that needs initializing.
 *  @return void
 */



void FIFO_Init(TFIFO * const FIFO){
    FIFO->Start=0;
    FIFO->End=0;
    FIFO->NbBytes=0;
}

/*! @brief Put one character into the FIFO.
 *
 *  @param FIFO A pointer to a FIFO struct where data is to be stored.
 *  @param data A byte of data to store in the FIFO buffer.
 *  @return bool - TRUE if data is successfully stored in the FIFO.
 *  @note Assumes that FIFO_Init has been called.
 */
bool FIFO_Put(TFIFO * const FIFO, const uint8_t data){
    if(FIFO->NbBytes<200){
	FIFO->Buffer[FIFO->End]=data;
	FIFO->End++;
	FIFO->NbBytes++;
	return true;
    }
    else{
	return false;
    }

}

/*! @brief Get one character from the FIFO.
 *
 *  @param FIFO A pointer to a FIFO struct with data to be retrieved.
 *  @param dataPtr A pointer to a memory location to place the retrieved byte.
 *  @return bool - TRUE if data is successfully retrieved from the FIFO.
 *  @note Assumes that FIFO_Init has been called.
 */
bool FIFO_Get(TFIFO * const FIFO, uint8_t * const dataPtr){
    if(FIFO->NbBytes==0){
	/*checks if empty*/

	return false;
    }

    else{
	*dataptr=FIFO->Buffer[FIFO->Start];
	FIFO->Start++;
	FIFO->NbBytes--;
	return true;
    }

}

