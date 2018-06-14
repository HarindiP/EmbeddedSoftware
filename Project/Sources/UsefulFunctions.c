#include <math.h>
#include <types.h>
//
//
#define NB_OF_SAMPLE 16
#define BITS_PER_VOLT 3276.7

int16_t Ts;
int16_t Sample[NB_OF_SAMPLE];
float x;


int16_t VRMS(int16_t* const Sample)
{
	int32_t v_rms;
	for (int i = 1; i<NB_OF_SAMPLE; i++)
	{
    v_rms += (Sample[i]) * (Sample[i]);
	}
	v_rms = v_rms/16;
	v_rms = (int16_t)sqrt((double)v_rms);
	return v_rms;
}

/*! @brief Converts analog values to voltage by find bits to volts
 *
 *  @param
 *  @note
 */
float AnalogtoVoltage(void)
{
  return x / BITS_PER_VOLT;
}


/*! @brief Converts analog values to voltage by find bits to volts
 *
 *  @param
 *  @note
 */
float VoltagetoAnalog(void)
{
  return x * BITS_PER_VOLT;
}





















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

