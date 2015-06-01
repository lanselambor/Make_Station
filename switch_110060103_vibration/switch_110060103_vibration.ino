/*
* Moisture Led.ino
* A demo for ChaiHuo ZaoWuBa Demo T14014
* 
* Copyright (c) 2015 Seeed Technology Inc.
* Auther     : lambor.Fang
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
#include "WatchDog.h"
#include <TimerOne.h>

#define DEBUG          0

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

#define SWITCH         A5
#define VIBRATOR       3
#define TIMEUP         1*60  //45 minutes

unsigned long timeCounter = 0;

void setup()
{

  WTD.watchdogSetup();
  WTD.doggieTickle();
    
  //power up
  pinMode(CHRG_LED, OUTPUT);
  digitalWrite(CHRG_LED, LOW);
  
  pinMode(SWITCH, INPUT);
  digitalWrite(SWITCH, HIGH);
  pinMode(VIBRATOR, OUTPUT);
  
  pinMode (10,OUTPUT);
  for(int i=0;i<2;i++)
  {
    analogWrite(10,5);
    delay(500);
    analogWrite(10,0);     
    delay(500);  
    WTD.doggieTickle();
  }
#if DEBUG  
  Serial.begin(9600);
#endif  
  Timer1.initialize(500000);//timing for 500ms
}

void TimingISR()
{
  timeCounter++;
#if DEBUG  
  Serial.println(digitalRead(SWITCH));
#if DEBUG  
}

void loop()
{
  if(HIGH == digitalRead(SWITCH))
  {
    vibrate(1);        
    while(HIGH == digitalRead(SWITCH))
    {
      WTD.doggieTickle();
      delay(50);
    }
  }
  else if(LOW == digitalRead(SWITCH))
  {
    Timer1.attachInterrupt(TimingISR);
    while(LOW == digitalRead(SWITCH))
    {
      if(TIMEUP < timeCounter/2)
      {
        vibrate(1);        
      } 
      WTD.doggieTickle();
    }    
    Timer1.detachInterrupt();    
    timeCounter = 0;
  }
  WTD.doggieTickle(); 
}

void vibrate(float freq)
{
  int ms = (int)freq / 2.0 * 1000;
  digitalWrite(VIBRATOR, HIGH);
  delay(ms);
  digitalWrite(VIBRATOR, LOW);
  delay(ms);
}

