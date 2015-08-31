/*
* Digital Sand Clock.ino
* A demo for ChaiHuo ZaoWuBa Demo
* 
* Copyright (c) 2015 Seeed Technology Inc.
* Auther     : Lmabor.Fang
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
#include <Wire.h>
#include <Arduino.h>
#include "GroveJoint.h"
#include <TimerOne.h>

#define Debug          0

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

#define LIGHT_UP_TIME 60000//  1 minute delay

int led = 3;  //led control output pin
int pir = A5;   //PIR_Sensor input pin

void pirNightLed();
void pirNightLedInit();
long cal_circle_y(long x, long r);
void ledOnOrOff(int status);

void setup() 
{
  //Joint hardware init
  Joint.initHardware();
  pirNightLedInit();
	
  WTD.watchdogSetup();
  WTD.doggieTickle();

#if Debug
  Serial.begin(9600);  
  Serial.println("start");
#endif  
}

void loop() 
{		
	//Pir motion sensor and color LED 
	pirNightLed();	
	
	//Joint battery manager	
    Joint.battery();
	
	//watch dog trickle 
	WTD.doggieTickle();	
}

void pirNightLed()
{  
  static bool ledOnFlag = false;
  static unsigned long begin = 0;
  int on = 1, off = 0;
    
  if(digitalRead(pir))
  {
    begin = millis();
    if(!ledOnFlag)
    {
      ledOnOrOff(on);
      ledOnFlag = true;
    }
  }
  if(millis() - begin >= LIGHT_UP_TIME)
  {    
    if(ledOnFlag)
    {
      ledOnOrOff(off);
      ledOnFlag = false;
    }
  }         
}

//preparation initialization
void pirNightLedInit()
{
  pinMode(led, OUTPUT);
  pinMode(pir, INPUT);
  analogWrite(led,255);
}

void ledOnOrOff(int status)
{
  if(status)
  {
    //turn on the light
    for(int i=255; i >= 0; i--)
    {
      long y = cal_circle_y(i, 255);   
      analogWrite(led,y);      	  	
	  WTD.doggieTickle();   //watch dog trickle 
      delay(10);      
    }
  }
  else
  {
    //Turn off the light
    for(int i = 0; i<=255; i++)
    {
      long y = cal_circle_y(i, 255);    
      analogWrite(led,y);     
	  WTD.doggieTickle();   //watch dog trickle 
      delay(10);      
    }
  }
}

//r * r = (x-r)*(x-r) + y * y 
long cal_circle_y(long x, long r)
{
    return sqrt((r * r) - (x - r) * (x - r));
}
