/*! @file Output.h
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

#ifndef SOURCES_OUTPUT_H_
#define SOURCES_OUTPUT_H_


#define OUTPUT_HIGH 5
#define OUTPUT_LOW 0

/*! @brief Raise the Alarm signal on chan 3
 *
 */
void Output_SetAlarm();

/*! @brief Clear the Alarm signal on chan 3
 *
 */
void Output_ClearAlarm();

/*! @brief Raise the Raise signal on chan 1
 *
 */
void Output_SetRaise();

/*! @brief Clear the Raise signal on chan 1
 *
 */
void Output_ClearRaise();

/*! @brief Raise the Lower signal on chan 2
 *
 */
void Output_SetLower();

/*! @brief Clear the Lower signal on chan 2
 *
 */
void Output_ClearLower();


#endif /* SOURCES_OUTPUT_H_ */

/*!
 ** @}
 */
