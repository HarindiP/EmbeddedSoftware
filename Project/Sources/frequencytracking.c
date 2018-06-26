/*
 * frequencytracking.c
 *
 *  Created on: 26 Jun 2018
 *      Author: 12443166
 */

#include "types.h"
#include "frequencytracking.h"
#include "math.h"
#define NB_OF_SAMPLES 16

int16_t myArray[16] = {-3, -8, 24, -1, 9, 12, -2, -4, 9, 15, -9, -7, 11, 7, 6, 5};

//check to see if between 2 zero crossing should be half the number of samples
static int16_t firstmin;
static int16_t secondmin;


static int16_t firstminpos;
static int16_t secondminpos;

#define ABS -1;


//calculates the minimum value of the array and their position in the array
void calcminimum(void)
{
  for(int i = 0; i < NB_OF_SAMPLES; i++)
  {
  if (myArray[i] < 0)
   {
     myArray[i] *= ABS
   }
  }

  // interate through the number of samples FOR Loop
  for(int i = 0; i < NB_OF_SAMPLES; i++)
  {
    if (i == 0)
    {
      firstmin = myArray[i];
      secondmin  =myArray[i+1];

      //initialises the positions of the 2 minimum values
      firstminpos = i;
      secondminpos = i+1;
    }

    else if (myArray[i] < firstmin)
    {
      secondmin = firstmin;
      firstmin = myArray[i];

      //keeps track of the position of the positio
      secondminpos = firstminpos;
      firstminpos = i+1;


    }
    else if ((myArray[i] > firstmin) && (myArray[i] <secondmin))
    {
      secondmin = myArray[i];
      secondminpos = i+1;
    }
  }
}



//once we have 2 minimums
