///*
// * voltageRegulator.c
// *
// *  Created on: 6 Jun 2018
// *      Author: 13181680
// */
//
//// 1. Three channels which regulate the actual voltage
//// 2. Calculate the average of the three channels
//// 3. Send the response (send the average??)
//
//#include "types.h"
//#include "analog.h"
//#include <types.h>
//
//#define UPPERBOUND 3
//#define LOWERBOUND 2
//
//int16_t *dataPtr;
//
//void readLoop(void)
//{
//  int16_t readvalue = 2.5;
//  while(readvalue > LOWERBOUND && readvalue < UPPERBOUND)
//  {
//    (void) Analog_Get(1, dataPtr);
//    readvalue = *dataPtr;
//  }
//
//  if(readvalue < LOWERBOUND) // Smaller than LOWERBOUND --> Assume PIT was initialized in main
//  {
//
//  }
//
//  else // Higher than the UPPERBOUND
//  {
//
//  }
//
//}
//
//
//void directmode(void);
//{
//  PIT_Set(5000000000, true); //5seconds timer start
//
//  //interupt produced that calls ISR that mkae changes to voltages depending on the boundary limits
//
//
//}
//
//void inversetimemode(void);
//{
//  //PIT_Set(5000000000, true);
//
//  FrequencyTracking();
//
//  InverseTimer();
//
//
//}
//
//void deviation(void)
//{
//
//}
//
//
//
