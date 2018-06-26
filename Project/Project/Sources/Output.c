/*! @file Output.c
 *
 *  @brief Output functions
 *
 *  This contains the functions used to manage the Alarm, Raise and Lower Output
 *
 *  @author Coralie
 *  @date 2018-05-20
 */
/*!
 **  @addtogroup Output_module Output module documentation
 **  @{
 */
/* MODULE Output */

#include "analog.h"
#include "Requirements.h"
#include "Output.h"



void Output_SetAlarm()
{
  Analog_Put(2,VOLT_TO_ANALOG(OUTPUT_HIGH));
}

void Output_ClearAlarm()
{
  Analog_Put(2,VOLT_TO_ANALOG(OUTPUT_LOW));
}

void Output_SetRaise()
{
  Analog_Put(0,VOLT_TO_ANALOG(OUTPUT_HIGH));
}

void Output_ClearRaise()
{
  Analog_Put(0,VOLT_TO_ANALOG(OUTPUT_LOW));
}

void Output_SetLower()
{
  Analog_Put(1,VOLT_TO_ANALOG(OUTPUT_HIGH));
}

void Output_ClearLower()
{
  Analog_Put(1,VOLT_TO_ANALOG(OUTPUT_LOW));
}


/*!
 ** @}
 */
