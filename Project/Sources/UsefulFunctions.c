//#include <math.h>
//#include <types.h>
//
//
//#define NB_OF_SAMPLE 16
//
//int16_t Ts;
//int16_t[16] Sample;
//
//uint16_t VRMS(int16* const Sample)
//{
//	uint32_t v_rms;
//	for (int i = 0; i<16; i++)
//	{
//		v_rms += (*(Sample+i)) * (*(Sample+i));
//	}
//	v_rms /= 16;
//	v_rms = (uint16_t)sqrt((double)v_rms);
//	retunr v_rms;
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
//
//uint8_t InverseTimer(int16_t deviation, int16_t* Ts)
//{
//	static uint8_t told;
//	uint8_t invTime;
//	invTime = (5 / (2 * deviation)) * (1 - (*Ts / told));
//	return invTime;
//}
//
//
