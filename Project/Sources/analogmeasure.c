/*! @file
 *
 *  @brief Functions for comparing and selecting minimum values read from ADC
 *
 *  analogMeasure.c
 *
 *  @author 13181680
 *  @date 6 Jun 2018
 */

 /*!
  *  @addtogroup analogMeasure_module analogMeasure module documentation
  *  @{
  */

#include "analog.h"
#include "types.h"
#include "stdio.h"
#include "stdlib.h"
#include "UsefulFunctions.h"
#include "PIT.h"
#include "voltageRegulator.h"
#include "analogmeasure.h"
#include "signals.h"


int16_t firstMin; // This is going to be the minimum one
int16_t firstMinAux;
int16_t secondMin; // This is going to be the next or previous value to the minimum one
int16_t secondMinAux;
int firstMinPosition; // Position in myArray of minimum positive value
int secondMinPosition; // Position in myArray of second minimum positive value
int16_t period; // The period value aux in int
float finalPeriod; // Period value in float
int arrayPosition;
int16_t vrmsValue;
int32_t measurementsFreq = 1250000;


//Initilising channel data
channeldata Samples[3];


/*! @brief Compares the new measured value with the ones already stored
 *
 *  @param value It is the new measured value
 *  @param position It is the position of the read value
 *  @note It is supposed that the ADC has been initialized
 */
//void compareMinimum(float value, int position)
//{
//  if(value < firstMin)
//  {
//    secondMin = firstMin;
//    firstMin = value;
//
//    secondMinPosition = firstMinPosition;
//    firstMinPosition = position;
//  }
//  else if(value > firstMin && value < secondMin)
//  {
//    secondMin = value;
//
//    secondMinPosition = position;
//  }
//}
//
//void calculation (int minimumPosition, int16_t minimum, int16_t minimumAux)
//{
//
//  // Special case for position zero
//
//  if(minimumPosition == 0)
//  {
//    if(myArray[minimumPosition] < 0)
//    {
//      if(myArray[minimumPosition + 1] > 0)
//      {
//        minimum = myArray[minimumPosition]; // Closest value to 0 --> negative
//        minimumAux = myArray[minimumPosition + 1]; // Consecutive value --> positive
//
//        period = measurementsFreq * (minimum / (abs(minimum) + minimumAux));
//        period += minimumPosition * measurementsFreq;
//        finalPeriod = (float)period;
//      }
//    }
//    if(myArray[minimumPosition] > 0)
//    {
//      if(myArray[minimumPosition + 1] < 0)
//      {
//        minimum = myArray[minimumPosition]; // Closest value to 0 --> positive
//        minimumAux = myArray[minimumPosition + 1]; // Next value --> negative
//
//        period = measurementsFreq * (minimum / (abs(minimum) + minimumAux));
//        period += minimumPosition * measurementsFreq;
//        finalPeriod = (float)period;
//      }
//    }
//    if(myArray[minimumPosition] == 0)
//    {
//      period = minimumPosition * measurementsFreq;
//      finalPeriod = (float)period;
//    }
//  }
//
//  // Special case for position 15
//
//  else if(minimumPosition == 15)
//    {
//      if(myArray[minimumPosition] < 0)
//      {
//        if(myArray[minimumPosition - 1] > 0)
//        {
//          minimum = myArray[minimumPosition]; // Closest value to 0 --> negative
//          minimumAux = myArray[minimumPosition - 1]; // Previous value --> positive
//
//          period = measurementsFreq * (minimum / (abs(minimum) + minimumAux));
//          period -= minimumPosition * measurementsFreq;
//          finalPeriod = (float)period;
//        }
//      }
//      if(myArray[minimumPosition] > 0)
//      {
//        if(myArray[minimumPosition - 1] < 0)
//        {
//          minimum = myArray[minimumPosition]; // Closest value to 0 --> positive
//          minimumAux = myArray[minimumPosition - 1]; // Previous value --> negative
//
//          period = measurementsFreq * (minimum / (abs(minimum) + minimumAux));
//          period -= minimumPosition * measurementsFreq;
//          finalPeriod = (float)period;
//        }
//      }
//      if(myArray[minimumPosition] == 0)
//      {
//        period = minimumPosition * measurementsFreq;
//        finalPeriod = (float)period;
//      }
//    }
//
//  // Remaining cases
//
//  else
//  {
//
//    if(myArray[minimumPosition] < 0)
//    {
//      if(myArray[minimumPosition + 1] > 0)
//      {
//        minimum = myArray[minimumPosition]; // Closest value to 0 --> negative
//        minimumAux = myArray[minimumPosition + 1]; // Consecutive value --> positive
//
//        period = measurementsFreq * (minimum / (abs(minimum) + minimumAux));
//        period += minimumPosition * measurementsFreq;
//        finalPeriod = (float)period;
//      }
//
//      if(myArray[minimumPosition + 1] < 0)
//      {
//        minimum = myArray[minimumPosition]; // Closest value to 0 --> negative
//        minimumAux = myArray[minimumPosition - 1]; // Previous value --> positive
//
//        period = measurementsFreq * (minimum / (abs(minimum) + minimumAux));
//        period -= minimumPosition * measurementsFreq;
//        finalPeriod = (float)period;
//      }
//    }
//
//    if(myArray[minimumPosition] > 0)
//    {
//      if(myArray[minimumPosition + 1] > 0)
//      {
//        minimum = myArray[minimumPosition]; // Closest value to 0 --> positive
//        minimumAux = myArray[minimumPosition - 1]; // Previous value --> negative
//
//        period = measurementsFreq * (minimum / (abs(minimum) + minimumAux));
//        period -= minimumPosition * measurementsFreq;
//        finalPeriod = (float)period;
//      }
//
//      if(myArray[minimumPosition + 1] < 0)
//      {
//        minimum = myArray[minimumPosition]; // Closest value to 0 --> positive
//        minimumAux = myArray[minimumPosition + 1]; // Next value --> negative
//
//        period = measurementsFreq * (minimum / (abs(minimum) + minimumAux));
//        period += minimumPosition * measurementsFreq;
//        finalPeriod = (float)period;
//      }
//    }
//
//    if(myArray[minimumPosition] == 0) // The value measured is exactly zero
//    {
//      period = minimumPosition * measurementsFreq;
//      finalPeriod = (float)period;
//    }
//  }
//}
//
///*! @brief Calculates the lowest value (absolute value) of the array and its position
// *
// *  @note It is supposed that the ADC has been initialized
// */
//void calculateMinimum(void)
//{
//  for(int i = 0; i < (sizeof(myArray)/sizeof(int16_t)); i++) // We check from second position to the previous of the last one
//  {
//    if (i==1) // We initialize firstMin to the second position of the array --> Enable to check minimum values
//    {
//      firstMin = abs(myArray[i]);
//      firstMinPosition = i;
//    }
//    else if (i==1) // We initialize secondMin to the second position of the array
//    {
//      secondMin = abs(myArray[i]);
//      secondMinPosition = i;
//      if(secondMin < firstMin) // We check if secondMin is smaller than firstMin
//      {
//        int16_t aux;
//        int auxpos;
//
//        //Values update
//        aux = firstMin;
//        firstMin = secondMin;
//        secondMin = aux;
//
//        //Position update
//        auxpos = firstMinPosition;
//        firstMinPosition = secondMinPosition;
//        secondMinPosition = auxpos;
//      }
//    }
//    else
//    {
//      compareMinimum(abs(myArray[i]), i); // New element value comparison
//    }
//  }
//
//  // End of for loop --> The two minimums are already calculated, as well as their position
//
//  int minimumPosition = firstMinPosition;
//  int16_t minimum = firstMin;
//  int16_t minimumAux = firstMinAux;
//
//  calculation(minimumPosition, minimum, minimumAux); // Calculation for the first minimum
//
//  minimumPosition = secondMinPosition;
//  minimum = secondMin;
//  minimumAux = secondMinAux;
//
//  calculation(minimumPosition, minimum, minimumAux); // Calculation for the second minimum
//
//}

/*! @brief It reads a value from the ADC and put it in myArray
 *
 *  @note It is supposed that the ADC has been initialized
 *  @note Needs to be called sixteen times to fill the array
 */
void UpdateInput(void)
{
  static int16_t tempval;
  static int16_t tempval1;
  static int16_t tempval2;

  Analog_Get(0,&(Samples[0].myArray[Samples[0].myposition]));



  arrayPosition++;
  if(arrayPosition == 16) // Reset position at the end of the array
  {
    arrayPosition = 0;
    tempval = VRMS(Samples[0].myArray);
    BoundsCheck(tempval);
    vrmsValue = tempval;

  }
}

/*!
 * @}
*/
