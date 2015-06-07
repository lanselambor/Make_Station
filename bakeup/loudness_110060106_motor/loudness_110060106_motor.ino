/*
* Digital Sand Clock.ino
* A demo for ChaiHuo ZaoWuBa Demo
* 
* Copyright (c) 2015 Seeed Technology Inc.
* Auther     : Lambor.Fang
* Create Time: May 2015
* Change Log :
* 
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/
#include <TimerOne.h>
#include "WatchDog.h"
#include <Wire.h>

#define DEBUG 0   //enable debug

#define BUTTON         2
#define LIGHT_SENSOR   A0
#define CHRG_LED       A3  //low-level work
#define PWR_HOLD       A1  
#define PWR            6   //low-level work
#define KEY            2
#define LED            10  
#define OUT_PIN1       3   //normal output pin
#define OUT_PIN2       5
#define IN_PIN1        A5  //normal input pin
#define IN_PIN2        A4

#define MOTOR_SPEED 20
#define TIMER_TIME 50000   //interrupt time 50ms
#define THRESHOLD 30

int sound_pin = A5;
int motor = 3;
volatile float sound_value = 0.0;
volatile float quiet_value = 0.0;
volatile bool Motor_Sleep = false;


void setup() 
{ 
  //power up
  pinMode(CHRG_LED, OUTPUT);
  digitalWrite(CHRG_LED, LOW);
  
  //initial WatchDog
  WTD.watchdogSetup();
  WTD.doggieTickle();

  Serial.begin(9600);
  pinMode(sound_pin, INPUT);
  pinMode(motor,OUTPUT);

  //calculate out a quiet starting average value 
  delay(100);
  long tmp = 0, sample_num = 1000;
  for(int i=0; i<sample_num; i++)
  {
    tmp += analogRead(sound_pin);
    delayMicroseconds(10);
  }
  quiet_value = tmp / sample_num;  
  
  //start light
  pinMode (10,OUTPUT);
  for(int i=0;i<2;i++)
  {
    analogWrite(10,5);
    delay(500);
    analogWrite(10,0);     
    delay(500);  
    WTD.doggieTickle();
  }
  
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
    sound_value = sound_value * 0.70 + analogRead(sound_pin) * 0.30;
}

void loop() 
{
  if( THRESHOLD < (sound_value - quiet_value ) )
  {
    analogWrite(motor, MOTOR_SPEED);
    delay(50);
  }
  analogWrite(motor, 0);   
  delay(80);
  
#if DEBUG  
  Data_acquisition(sound_value,sound_value - quiet_value,quiet_value,0);
#endif  
  WTD.doggieTickle();
  delay(20);
}

#if DEBUG
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

