/*
 * frequencytracking.c
 *
 *  Created on: 26 Jun 2018
 *      Author: 12443166
 */

#include "types.h"
#include "frequencytracking.h"
#include "math.h"
#include "string.h"


#define ABS -1;
#define NB_OF_SAMPLES 16

channeldata Samples[3];

//int16_t myArray[16] = {-3, -8, -4, -1, 9, 12, 2, 4, 9, 15, 9, 7, -11, -7, -6, -5};

//check to see if between 2 zero crossing should be half the number of samples
static int16_t firstmin;
static int16_t secondmin;


static int16_t firstminpos;
static int16_t secondminpos;


//variable to save the value of the variable the crosses the zero crossing
static int16_t consecMinOne;


int32_t freqmeasurement = 1250000;




//calculates the minimum value of the array and their position in the array
void calcminimum(void)
{
//  int16_t myArray[16] = {-3, -8, -4, -1, 9, 12, 2, 4, 9, 15, 9, 7, -11, -7, -6, -5};
  channeldata Samples[3];
  for(int i = 0; i < NB_OF_SAMPLES; i++)
  {
    if((Samples[0].myArray[i]) < 0 )
    {
      Samples[0].myArray[i] *= -1;
    }

  }

  // interate through the number of samples FOR Loop
  for(int i = 0; i < NB_OF_SAMPLES; i++)
  {
    if (i == 0)
    {
      firstmin = Samples[0].myArray[i];
      secondmin  = Samples[0].myArray[i+1];

      //initialises the positions of the 2 minimum values
      firstminpos = i;
      secondminpos = i+1;
    }

    else if (Samples[0].myArray[i] < firstmin)
    {
      secondmin = firstmin;
      firstmin = Samples[0].myArray[i];

      //keeps track of the position of the positio
      secondminpos = firstminpos;
      firstminpos = i;


    }
    else if ((Samples[0].myArray[i] > firstmin) && (Samples[0].myArray[i] <secondmin))
    {
      secondmin = Samples[0].myArray[i];
      secondminpos = i;
    }
  }
  ZeroCrossing();
}


void ZeroCrossing(void)
{
  //this bit better fucking find if both variable have opposites signs

  if ((firstminpos > 0) && (firstminpos < 16))
  {
        if((Samples[0].myArray[firstminpos] > 0) && (Samples[0].myArray[firstminpos + 1] < 0))
          consecMinOne = Samples[0].myArray[firstminpos - 1];

        else if((Samples[0].myArray[firstminpos] > 0) && (Samples[0].myArray[firstminpos - 1] < 0))
          consecMinOne = Samples[0].myArray[firstminpos - 1];

        else if((Samples[0].myArray[firstminpos] < 0) && (Samples[0].myArray[firstminpos + 1] > 0))
          consecMinOne = Samples[0].myArray[firstminpos + 1];

        else if((Samples[0].myArray[firstminpos] < 0) && (Samples[0].myArray[firstminpos - 1] > 0))
          consecMinOne = Samples[0].myArray[firstminpos - 1];
  }
}

//interpolation function thats takes in the 2 minimum values and there positions in the array
//void InterpoleMeDad(void)
//{
//  //sum of the minimum values
//  float firstmin;
//  float consecMinOne; //the values of minimum and the value that crosses zero axis bois
//  float frac = firstmin / (fabs(firstmin) + fabs(consecMinOne));
//  float crossingpoint = freqmeasurement * frac;
//
//  //if firstmin and secondmoin
//  float timeofcrossing = freqmeasurement * (firstmin + crossingpoint);
//
//  //timeofcrossing = freqmeasurement * (minimum position + or - crossing point)
//
//  //return timeofcrossing;
//
//}



//once we have 2 minimums
