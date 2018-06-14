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

float myArray[16] = {1,2,3.5,4,6,0.4,7,8,5,4,2,1,-0.7,-0.44,-0.8,-4}; // Array of the total of elements measured
float firstMin; // This is going to be the minimum one (absolute value)
float secondMin; // This is going to be the second minimum one (absolute value)
int firstMinPosition; // Position in myArray of firstMin
int secondMinPosition; // Position in myArray of the secondMin
float period; // deviation
int sampleSize; // Number of values to take into account from the array for measuring
int globalPos; // Position to store new data in the global array
//float temp[16] =   // 5 and 14


const uint8_t channelNb = 1;
//int16_t* const valuePtr;  //dont need yet???
float * const WritePtr;  //pointer to begining of my array



/*! @brief Compares the new measured value with the ones already stored
 *
 *  @param value It is the new minimum value
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
 *  @note interpoling function
 */
float calculateMinimum(void)
{
  for(int i = 0; i < (sizeof(myArray)/ sizeof(int16_t)); i++)
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

  // Assuming the for loop have as already found the minimum value and its position

  if (myArray[firstMinPosition] > 0) //value at index value is positive
  {
    // if the positon after is postive

    firstMin = myArray[firstMinPosition];
    secondMin =  myArray[firstMinPosition - 1];

    period = 1250000 * (firstMin  /abs(firstMin) + secondMin);
    period -= 1250000 * firstMinPosition;


    //if the second positon after is negative
    firstMin = myArray[firstMinPosition];
    secondMin =  myArray[firstMinPosition + 1];

    period = 1250000 * (firstMin  /abs(firstMin) + secondMin);
    period += 1250000 * firstMinPosition;

  }

  else if(myArray[firstMinPosition] < 0) //value at index value is positive
  {
    // if the positon after is postive
    firstMin = myArray[firstMinPosition];
    secondMin =  myArray[firstMinPosition + 1];

    period = 1250000 * (firstMin  /abs(firstMin) + secondMin);
    period += 1250000 * firstMinPosition;

    // if the positon after is negative
    firstMin = myArray[firstMinPosition];
    secondMin =  myArray[firstMinPosition - 1];

    period = 1250000 * (firstMin  /abs(firstMin) + secondMin);
    period -= 1250000 * firstMinPosition;

  }
  else
  {
    period = 1250000* firstMinPosition;  //deviation from x axis
  }

  return firstMin;
}


/*! @brief Writes into my array from ADC
 *
 *  @param
 *  @note It is supposed that the ADC has been initialized
 */

void WriteInput (void)
{
  (void) Analog_Get(channelNb, WritePtr);
  myArray[globalPos] = AnalogtoVoltage(*WritePtr);

  globalPos++; //want to make sure called 16 times


  //intialises the position
  if (globalPos == 16)
  {
    globalPos = 0;
  }
}

// creat function that will ease the calculate minumum value

int main (void)
{
  calculateMinimum();
  printf("%d" , calculateMinimum());

}
/*!
 * @}
*/





