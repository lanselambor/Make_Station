/*
* Coin counter.ino
* A demo for ChaiHuo ZaoWuBa Demo T14015
* 
* Copyright (c) 2015 Seeed Technology Inc.
* Auther     : Lmabor.Fang
* Create Time: May 2015
* Change Log : Lambor modified and update at May 2015
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
#include <Wire.h>

#define  DeBug   0

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

#define LINE_FINDER_FIND       (LOW == digitalRead(IN_PIN1))   //something cover the sensor
#define LINE_FINDER_NOTFIND    (HIGH == digitalRead(IN_PIN1)) //nothing cover the sensor
#define TIME_TO_DRINK          2700000   //unit: Ms. 2700000 Ms = 45 Min, time for not drinking water 
#define SPEAK_DURATION         4         //2 s

const int linefinder               = IN_PIN1;
const int recorder                 = OUT_PIN1;

volatile unsigned long timeCounter = 0;
void setup()
{  
  //power up
  pinMode(CHRG_LED, OUTPUT);
  digitalWrite(CHRG_LED, LOW);
  
  //initial watchdog   
  WTD.watchdogSetup();
  WTD.doggieTickle();
  
  //initial devices
  pinMode(linefinder,INPUT);    
  pinMode(recorder, OUTPUT);
  digitalWrite(recorder, LOW);
  
  LEDShine(2, 1000);
  
#if DeBug  
  Serial.begin(9600);
  Serial.println("start");
#endif    
  Timer1.initialize(50000);//timing for 50ms
}

void TimingISR(void)
{   
  timeCounter++;
}

void loop()
{      
  if (LINE_FINDER_NOTFIND)
  {
    speak(1, SPEAK_DURATION);
    while(LINE_FINDER_NOTFIND)
    {
      WTD.doggieTickle();
      delay(5);
    }
  }
  else if (LINE_FINDER_FIND)
  {

    Timer1.attachInterrupt(TimingISR);
    while(LINE_FINDER_FIND)
    {
      if( timeCounter * 50 > TIME_TO_DRINK )
      {
        #if DeBug
        Serial.print("duration: ");
        Serial.println(timeCounter * 50);
        #endif
        speak(3, SPEAK_DURATION);        
        timeCounter = 0;
      } 
      WTD.doggieTickle();
    }
    Timer1.detachInterrupt();
    timeCounter = 0;
  }            
}

/* Function   : void LEDShine(int times, int freqMs)
 * parameters : times: counting times of shining;
 *              freqMs: shining frequency in microseconds
 */
void LEDShine(int times, int freqMs)
{
  pinMode (LED,OUTPUT);
  for(int i=0;i<times;i++)
  {
        analogWrite(LED,5);
        delay(freqMs/2);
        analogWrite(LED,0);     
        delay(freqMs/2);  
        WTD.doggieTickle();
  }
}

void speak(int times, int seconds)
{
  for(int i=0; i<times; i++)
  {
    
    digitalWrite(recorder, HIGH);
    delaySeconds(seconds);    
    digitalWrite(recorder, LOW);
    delaySeconds(1);

  }
}

void delaySeconds(int seconds)
{
  for(int j=0; j<seconds; j++)  //delay seconds
  {
    WTD.doggieTickle(); 
    delay(1000);
  }
}