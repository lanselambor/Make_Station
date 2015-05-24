#ifndef _VISUALSCOPE_H_
#define _VISUALSCOPE_H_

#include <Arduino.h>


class VisualScope
{
	public:
		/*--------------------- Calculate CRC16 check number -----------------*/
		unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT);		
		/*------------------ Serial output data --------------------------*/
		void OutPut_Data(void);
		/*------------------------ Data in ---------------------------------*/
		void Data_acquisition(float temp1,float temp2,float temp3,float temp4);	 
	private:
		float OutData[4];
};

#endif