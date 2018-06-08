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

int16_t myArray[16]; // Array of the total of elements measured
int16_t firstMin; // This is going to be the minimum one (absolute value)
int16_t secondMin; // This is going to be the second minimum one (absolute value)
int firstMinPosition; // Position in myArray of firstMin
int secondMinPosition; // Position in myArray of the secondMin
int sampleSize; // Number of values to take into account from the array for measuring
int globalPos; // Position to store new data in the global array

const uint8_t channelNb = 1;
int16_t* const valuePtr;  //dont need yet???
int16_t * const WritePtr;  //pointer to begining of my array



/*! @brief Compares the new measured value with the ones already stored
 *
 *  @param value It is the new measured value
 *  @note It is supposed that the ADC has been initialized
 */
void compareMinimum(int16_t value, int position)
{
  if(value < firstMin)
  {
    secondMin = firstMin;
    firstMin = value;

    secondMinPosition = firstMinPosition;
    firstMinPosition = position;
  }
  else if(value > firstMin && value < secondMin)
  {
    secondMin = value;

    secondMinPosition = position;
  }
}

/*! @brief Calculates the two lowest values (absolute value) of the array and their position
 *
 *  @note It is supposed that the ADC has been initialized
 */
void calculateMinimum(void)
{
  for(int i = 0; i < (sizeof(myArray)/sizeof(int16_t)); i++)
  {
//    (void) Analog_Get(channelNb, valuePtr);
//    myArray[i] = *valuePtr;]

    if (i==0) // We initialize firstMin to the first position of the array
    {
      firstMin = abs(myArray[i]);
      firstMinPosition = i;
    }
    else if (i==1) // We initialize secondMin to the second position of the array
    {
      secondMin = abs(myArray[i]);
      secondMinPosition = i;
      if(secondMin < firstMin) // We check if secondMin is smaller than firstMin
      {
        int16_t updatevar; //update the new variable that helps save
        int updatevarpos;

        //Values update
        updatevar = firstMin;
        firstMin = secondMin;
        secondMin = updatevar;

        //Position update
        updatevarpos = firstMinPosition;
        firstMinPosition = secondMinPosition;
        secondMinPosition = updatevarpos;
      }
    }
    else
    {
      compareMinimum(abs(myArray[i]), i); // New element value comparison
    }
  }

  // End of for loop --> Two minimum values already calculated, as well as their positions

  sampleSize = firstMinPosition - secondMinPosition;
  sampleSize = abs(sampleSize); // Number of values of the array which define the half-window


}

/*! @brief Writes into my array
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */

void InputWrite (void)
{
  (void) Analog_Get(channelNb, valuePtr);
  myArray[globalPos] = *valuePtr;
  globalPos++; //want to make sure called 16 times

}
/*!
 * @}
*/





