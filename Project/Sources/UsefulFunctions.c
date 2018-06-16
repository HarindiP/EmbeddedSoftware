#include <math.h>
#include <types.h>
//
//
#define NB_OF_SAMPLE 16
#define BITS_PER_VOLT 3276.7  /* ie. each increment is an increase of 1V*/

int16_t Ts;
float Sample[NB_OF_SAMPLE];
float value;


float VRMS(float Sample[])
{
	float v_rms;
	// what if there are less than 16 samples
	for (int i = 1; i<NB_OF_SAMPLE; i++)
	{
    v_rms += (Sample[i]) * (Sample[i]);
	}
	v_rms = (float)v_rms/16;
	v_rms = (float)sqrt((float)v_rms);

	//what if vrms is 0

	return v_rms;
}

/*! @brief Converts analog values to voltage by  bits to volts
 *
 *  @param
 *  @note maybe static??
 */
float AnalogtoVoltage(float value)
{
//  if (value < 0)
//  {
//    value *= -1;
//  }
  return value / BITS_PER_VOLT;
}


/*! @brief Converts analog values to voltage by  bits to volts
 *
 *  @param
 *  @note maybe static??
 */

float VoltagetoAnalog(float value)
{
//  if valuex < 0)
//  {
//    value *= -1;
//  }

  return value * BITS_PER_VOLT;
}


/*! @brief uses to calulate new pit period by using zero crossing and time between
 *
 *  @param
 *  @note maybe static??
 */

//void frequencytracking ( ThreaddataStructtype channeldata, uint8_t count)
//{
//  if (channeldata->channelNb == 0)
//  {
//    if (sample[count]>0 && samples[count-1] < 0)
//    {
//      case for first zero crossing
//         calculate the timeoffset between the one before and the one after
//        reset the sampleoffsett
//        look for second zero crossing
//        break
//
//      case for 2nd zerocrossing
//        calculate the the off set from the the two samples
//
//      newperiod = (sampleoffset-(sum of offsets)) * (periodinns/ 1000000000)
//      newfreq = 1/newperiod
//
//
//      if (newfreq>=47.5 and newfreq<=52.5)
//      {
//        //set in tp global freq
//
//        periodinns = ((1/ newfreq)/16) * 1000000000; //peirod in nanesecs
//
//        PIT_Set(periodinns, true);
//      }
//
//      //initialise the crossing variable
//      break;
//
//      //defaultcase: break;
//
//
//
//    }
//    //increment sample offset
//  }
//}



//float timeoffset(float value1 , float value2)
//{
//  float m = (value2 - value1 ) / (value2 + value1)
//  float offset = (-value1) / m ;
//  return offset;
//
//}














//
//int16_t Interpolation(int16_t x1, int16_t x2, int16_t y1, int16_t y2)
//{
//	int16_t p = (y2 - y1) / (x2 - x1);
//	int16_t y = ((-y1) / p) + x1;
//	return y;
//}
//
//void FrequencyTracking(int16_t* const sample, int16_t* Ts)
//{
//	uint16_t k = 0;
//	while(k < 2)
//	{
//		uint16_t[2] zeros;
//		for(int i = 0; i < NB_OF_SAMPLE - 1; i++)
//		{
//			if(*(Sample+i) == 0 )
//			{
//				zeros[k] = *Ts * i / NB_OF_SAMPLE;
//				k++;
//			}
//			if(*(Sample+i+1) == 0 )
//			{
//				zeros[k] = *Ts * (i + 1) / NB_OF_SAMPLE;
//				k++;
//			}
//			if ((*(Sample+i)) * (*(Sample+i+1)) < 0)
//			{
//				zero[k] =  Interpolation(*(Sample+i),*(Sample+i+1),Ts * i / NB_OF_SAMPLE,*Ts * (i + 1) / NB_OF_SAMPLE);
//			}
//		}
//	}
//	if ((zero[1] - zero[0]) != (Ts / 2))
//	{
//		*Ts = 2 * (zero[1] - zero[0]) / 16;
//	}
//}
////
//
//
//
//uint8_t InverseTimer(int16_t dev, int16_t* Ts)
//{
//	static uint8_t told;
//	uint8_t invTime;
//	invTime = (5 / (2 * dev)) * (1 - (*Ts / told));
//	return invTime;
//}
////
////

