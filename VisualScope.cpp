#include "VisualScope.h"


/*--------------------- Calculate CRC16 check number -----------------*/

unsigned short VisualScope::CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT)
{
	unsigned short CRC_Temp;
	unsigned char i,j;
	CRC_Temp = 0xffff;

	for (i=0;i<CRC_CNT; i++)
	{      
		CRC_Temp ^= Buf[i];
		for (j=0;j<8;j++)
		{
			if (CRC_Temp & 0x01)
				CRC_Temp = (CRC_Temp >>1 ) ^ 0xa001;
			else
				CRC_Temp = CRC_Temp >> 1;
		}
	}
	return(CRC_Temp);
}


/*------------------ Serial output data --------------------------*/
void VisualScope::OutPut_Data(void)
{
  int temp[4] = {0};
  unsigned int temp1[4] = {0};
  unsigned char databuf[10] = {0};
  int i;
  unsigned short CRC16 = 0;
  for(i=0;i<4;i++)
   {
	
	temp[i]  = (int)OutData[i];
	temp1[i] = (unsigned int)temp[i];
	
   }
   
  for(i=0;i<4;i++) 
  {
	databuf[i*2]   = (unsigned char)(temp1[i]%256);
	databuf[i*2+1] = (unsigned char)(temp1[i]/256);
  }
  
  CRC16 = CRC_CHECK(databuf,8);
  databuf[8] = CRC16%256;
  databuf[9] = CRC16/256;
  
  for(i=0;i<10;i++)
  {
	 Serial.write(databuf[i]);
  }

}


/*------------------------ Data in ---------------------------------*/
void VisualScope::Data_acquisition(float temp1,float temp2,float temp3,float temp4)
{
	   OutData[0] = temp1;
	   OutData[1] = temp2;
	   OutData[2] = temp3; 
	   OutData[3] = temp4;
	   OutPut_Data();
}