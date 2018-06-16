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
#include "analogmeasure.h"


/*! @brief Compares the new measured value with the ones already stored
 *
 *  @param value It is the new minimum value
 *  @note It is supposed that the ADC has been initialized
 *  @note make bool to confirm that it is successfull
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

void UpdateInput (float * const WritePtr)
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

///*! @brief Writes into my array from ADC
// *
// *  @param
// *  @note It is supposed that the ADC has been initialized
// */
//void Updates(const int8_t channelNb, int16_t *value)
//{
//
//
//
//
//}


/*!
 * @}
*/

int main (void){
  calculateMinimum();
  printf("%f", firstMin);
  printf("%d", firstMinPosition);
  return 0;
}





