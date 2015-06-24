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

#define LIGHT_UP_TIME 60000//10*60*1000  1 minute delay

int led = 3;  //led control output pin
int pir = A5;   //PIR_Sensor input pin

void setup() {
  WTD.watchdogSetup();
  WTD.doggieTickle();
  
  pinMode(CHRG_LED, OUTPUT);
  digitalWrite(CHRG_LED, LOW);
  
  pinMode (10,OUTPUT);
  for(int i=0;i<2;i++)
  {
    analogWrite(10,5);
    delay(500);
    analogWrite(10,0);     
    delay(500);  
    WTD.doggieTickle();
  }
  
  pinMode(led, OUTPUT);
  pinMode(pir, INPUT);
  analogWrite(led,255);
  delay(500);
}

void loop() {
  if(digitalRead(pir))
  {    
    //turn on the light
    for(int i=255; i >= 0; i--)
    {
      long y = cal_circle_y(i, 255);   
      analogWrite(led,y);
      
      WTD.doggieTickle();
      delay(10);      
    }   
    
    unsigned long begin = millis();
    
    while(LIGHT_UP_TIME > millis() - begin)
    {
      if(digitalRead(pir))
      {
        begin = millis();
      }
      
      WTD.doggieTickle();  
      delay(10);
    }
    
    //Turn off the light
    for(int i = 0; i<=255; i++)
    {
      long y = cal_circle_y(i, 255);    
      analogWrite(led,y);
      
      WTD.doggieTickle();
      delay(10);      
    }
  } 
  
  delay(20);      
  WTD.doggieTickle();
}

//r * r = (x-r)*(x-r) + y * y 
long cal_circle_y(long x, long r)
{
    return sqrt((r * r) - (x - r) * (x - r));
}