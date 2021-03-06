/*
* Digital Sand Clock.ino
* A demo for ChaiHuo ZaoWuBa Demo
* 
* Copyright (c) 2015 Seeed Technology Inc.
* Auther     : Wayen.Weng
* Create Time: Jan 07, 2015
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
#include <EEPROM.h>
#include "MyTM1637.h"
#include "WatchDog.h"
#include <Wire.h>

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

#define TILT_PIN      A5
#define CLK           3
#define DIO           5
#define ON            1
#define OFF           0

MyTM1637 tm1637(CLK,DIO);

unsigned char MinuteMax = 25;
int8_t TimeDisp[] = {0,0,0,0};
unsigned char ClockPoint = 1;
unsigned char Update;
unsigned char halfsecond = 0;
unsigned char second = 0;
unsigned char minute = 0;
unsigned char displayflag = 0;
unsigned char tilt_value;

void setup()
{
  WTD.watchdogSetup();
  WTD.doggieTickle();
  
  pinMode(CHRG_LED, OUTPUT);
  digitalWrite(CHRG_LED, LOW);
  
  pinMode (10,OUTPUT);
  for(int i=0;i<2;i++)
  {
    analogWrite(10, 5);
    delay(500);
    analogWrite(10, 0);     
    delay(500);  
    WTD.doggieTickle();
  }
  
  Serial.begin(9600);
  Serial.print("Digital sand clock Test.\r\n"); 

  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);  
  tm1637.clearDisplay();

  pinMode(TILT_PIN,INPUT_PULLUP); 
  
  Timer1.initialize(500000);//timing for 500ms
  Timer1.attachInterrupt(TimingISR);//declare the interrupt serve routine:TimingISR  

  EEPROM.write(0,MinuteMax); //Setting minuteMax, the initial value of count time  max.

  MinuteMax = EEPROM.read(0);
  Serial.print("Read time max is: ");
  Serial.print(MinuteMax);
  Serial.print(" minutes.\r\n");

  minute = MinuteMax;  
}

void loop()
{ 
  if(tilt_value != digitalRead(TILT_PIN))
  {
    tilt_value = digitalRead(TILT_PIN);
    minute = MinuteMax;
    second = 0;
    displayflag = 1;
  }
  
  if(Update == ON)
  {
    TimeUpdate();
    if(tilt_value == 0) tm1637.display(TimeDisp,DISPLAY_FLAG_F);
    if(tilt_value == 1) tm1637.display(TimeDisp,DISPLAY_FLAG_B);
  } 
  
  if((minute == 0) && (second == 0))
  {
    displayflag = 0;
    ClockPoint = 1;
    minute = 0;
    second = 0;
    TimeUpdate();
    tm1637.display(TimeDisp,DISPLAY_FLAG_F);
  }   
  
#if 0  
  /** Here you type in a number as minutes,
    * for the 4 digital display can only show the largest time 99:59,
    * you should type in numbers between 0000 ~ 0100.
    */         
  String inString = "";
  unsigned int timeValue = 0;
  unsigned char bitCount = 0; 
  unsigned char dataTemp1[4] = {0};
  while(Serial.available() > 0)
  {
    unsigned char inChar = Serial.read();
    inString += (char)inChar;
    dataTemp1[bitCount] = inChar - '0';
    bitCount += 1;
    delay(10);        
  }
  if(inString != "")
  {
    //Serial.print("Into time max setup.\r\n");
    //delay(100);
    
    if(bitCount > 4)
    {
        Serial.print("Time input error.\r\n");
    }
    else
    {
      for(char i=0;i<bitCount;i++)
      {
        unsigned int dataTemp2 = 1;
        for(char j=0;j<(bitCount-i-1);j++)dataTemp2 *= 10;
        timeValue += (dataTemp1[i] * dataTemp2);
      }       
      if(timeValue != MinuteMax)
      {
        MinuteMax = timeValue;
        minute = MinuteMax; 
        second = 0;
        EEPROM.write(0,MinuteMax);
        Serial.print("Write the time max: ");
        Serial.print(MinuteMax);
        Serial.print(" minutes.\r\n");
      }  
    }
  }  
#endif
  
  WTD.doggieTickle();  
}

void TimingISR()
{
  if(displayflag == 1)
  {
    halfsecond ++;
    Update = ON;
    if(halfsecond == 2)
    {            
      if(second == 0)
      {
        if(minute == 0)
        {
            //minute = 25;
        }
        minute --;
        second = 60;
      }
      second --;
      halfsecond = 0;  
    }
    ClockPoint = (~ClockPoint) & 0x01;
  }
  
  WTD.doggieTickle();   
}

void TimeUpdate(void)
{
  if(ClockPoint)tm1637.point(POINT_ON);
  else tm1637.point(POINT_OFF); 
  TimeDisp[0] = minute / 10;
  TimeDisp[1] = minute % 10;
  TimeDisp[2] = second / 10;
  TimeDisp[3] = second % 10;
  Update = OFF;
}
