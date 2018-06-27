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

//variables to hold the min values in array
static int16_t firstmin;
static int16_t secondmin;

//variable to hold the minimum values position
static int16_t firstminpos;
static int16_t secondminpos;

//global variable with the 2 zero crossings

int32_t freqmeasurement = 1250000;

int32_t Zerocrossing1;
int32_t Zerocrossing2;

extern float frequency;

//calculates the minimum value of the array and their position in the array
void calcminimum(void)
{
  channeldata Samples[3];


  /********************************abs value**********************************************/
  for(int i = 0; i < NB_OF_SAMPLES; i++)
  {
    if((Samples[0].myArray[i]) < 0 )
    {
      Samples[0].myArray[i] *= -1;
    }
  }
  /**************************************************************************************/
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

      //keeps track of the positions
      secondminpos = firstminpos;
      firstminpos = i;


    }
    else if ((Samples[0].myArray[i] > firstmin) && (Samples[0].myArray[i] <secondmin))
    {
      secondmin = Samples[0].myArray[i];
      secondminpos = i;
    }
  }

  // Assume that the 2 closest values to the x axis is found
  /***************************************************************************************/
  //find the second value that crosses the x-axis
  int16_t Min1aux = AdjecentFind(firstminpos);
  int16_t Min2Aux = AdjecentFind(secondminpos);

  //find the new zero crossing between the postive and negative value
  Zerocrossing1 = Interpolation(firstmin, Min1aux);
  Zerocrossing2 = Interpolation(secondmin,Min2Aux);


  /***************************************************************************************/

}


int16_t AdjecentFind(int16_t position)
{
  //4 if statements that check to see if the which adjecent value has an opposite sign
  int16_t minAux;

  if ((position > 0) && (position < 16))
  {
        if((Samples[0].myArray[position] > 0) && (Samples[0].myArray[position + 1] < 0))
          return minAux = Samples[0].myArray[position - 1];

        else if((Samples[0].myArray[position] > 0) && (Samples[0].myArray[position - 1] < 0))
          return minAux = Samples[0].myArray[position - 1];

        else if((Samples[0].myArray[position] < 0) && (Samples[0].myArray[position + 1] > 0))
          return minAux = Samples[0].myArray[position + 1];

        else if((Samples[0].myArray[position] < 0) && (Samples[0].myArray[position - 1] > 0))
          return minAux = Samples[0].myArray[position - 1];
  }
}

//interpolation function thats takes in the 2 minimum values find zero crossing
int32_t Interpolation(int16_t min, int16_t minaux)
{
  int16_t crossingpoint;
  int16_t timeofcrossing;

  crossingpoint = (min / ((min) + (minaux))) * freqmeasurement;

  if ((min) || (min > 0 && minaux <0))
  {
    timeofcrossing = (freqmeasurement * min) - crossingpoint;
    return (int32_t)timeofcrossing;
  }
  else
  {
    timeofcrossing = (freqmeasurement * min) + crossingpoint;
    return (int32_t)timeofcrossing;
  }
}


void freqtracking(void)
{
  int32_t zero1 = Zerocrossing1;
  int32_t zero2 = Zerocrossing2;

  /***************************************/
  if (zero1 < 0 )
  {
    zero1 *= -1;
  }

  if (zero2 < 0 )
    {
      zero2 *= -1;
    }
  /****************************************/

  int32_t halfperiod = zero2 - zero1;

  if (halfperiod == 0)
  {
    frequency = 50;
    freqmeasurement = 1250000;
  }
  else
  {
    int32_t fullperiod = halfperiod * 2;

    //calculates new sampling period
    freqmeasurement = fullperiod/16;

    //time in seconds
    float periodsinsec= (float)fullperiod/ 1000000000;
    frequency = 1 / periodsinsec;

  }
  //Makes sure frequency is in bounds
  if (frequency < 47.5 || frequency > 52.5)
  {
    //set to defaults values
    frequency = 50;
    freqmeasurement = 1250000;
  }





}
