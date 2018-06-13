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
    firstMin = value;
    firstMinPosition = position;
  }
}

/*! @brief Calculates the two lowest values (absolute value) of the array and their position
 *
 *  @note It is supposed that the ADC has been initialized
 */
void calculateMinimum(void)
{
  for(int i = 1; i < (sizeof(myArray)/ sizeof(int16_t)) - 1; i++)
  {

    if (i==1) // We initialize firstMin to the first position of the array
    {
      firstMin = abs(myArray[i]);
      firstMinPosition = i;
    }
    else
    {
      compareMinimum(abs(myArray[i]), i); // New element value comparison
    }
  }

  // End of for loop -->  minimum value already calculated, as well as its positions

  if (myArray[firstMinPosition] > 0) //value at index value is positive
  {
    firstMin = myArray[firstMinPosition];
    secondMin =  myArray[firstMinPosition - 1];

  }

  else if(myArray[firstMinPosition] < 0) //value at index value is positive
  {
    firstMin = myArray[firstMinPosition];
    secondMin =  myArray[firstMinPosition + 1];
  }
  else
  {
    firstMin = myArray[firstMinPosition];
  }

}


/*! @brief Writes into my array from ADC
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */

void WriteInput (void)
{
  (void) Analog_Get(channelNb, valuePtr);
  myArray[globalPos] = AnalogtoVoltage(*valuePtr);

  globalPos++; //want to make sure called 16 times


  //intialises the position
  if (globalPos == 16)
  {
    globalPos = 0;
  }
}




/*!
 * @}
*/





