#include <TimerOne.h>

#define VS 1

#define MOTOR_SPEED 2
#define TIMER_TIME 50000   //interrupt time 50ms
#define THRESHOLD 120

int loudness = A5;
int motor = 3;
static float dat = 0;
volatile bool Motor_Sleep = false;

void setup() {  
  Serial.begin(9600);
  pinMode(loudness, INPUT);
  pinMode(motor,OUTPUT);
  
  dat = analogRead(loudness);
  
  Timer1.initialize(TIMER_TIME);//timing for 50ms
  Timer1.attachInterrupt(TimingISR);//declare the interrupt serve routine:TimingISR  
}

void TimingISR(void)
{   
    static int cnt = 0;
    const int TIMEOUT = 500;  //500ms
    cnt++;
    if(TIMEOUT == cnt * 50)    
    {
        Motor_Sleep = true;
        cnt = 0;
    }
    dat = dat * 0.70 + analogRead(loudness) * 0.30;
    
}

void loop() {
  if( THRESHOLD < dat )
  {
    analogWrite(motor, MOTOR_SPEED);
    delay(50);
  }
  analogWrite(motor, 0);   
  delay(80);
  
#if VS  
  Data_acquisition(dat,0,0,0);
#endif  

}

#if VS
float OutData[4]={0};

unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT)
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

void OutPut_Data(void)
{
  int temp[4] = {0};
  unsigned int temp1[4] = {0};
  unsigned char databuf[10] = {0};
  uint8_t i;
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

void Data_acquisition(float temp1,float temp2,float temp3,float temp4)
{
   OutData[0] = temp1;
   OutData[1] = temp2;
   OutData[2] = temp3; 
   OutData[3] = temp4;
   OutPut_Data();
}

#endif

