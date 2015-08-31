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
#include <Arduino.h>
#include "GroveJoint.h"

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
#define TIMEUP         60//1*60  //45 minutes

unsigned long timeCounter = 0;

void setup()
{
  //Joint hardware init
  Joint.initHardware();
  
  WTD.watchdogSetup();
  WTD.doggieTickle();    
  
  pinMode(SWITCH, INPUT);
  digitalWrite(SWITCH, HIGH);
  pinMode(VIBRATOR, OUTPUT);
  
#if DEBUG  
  Serial.begin(9600);
  Serial.println("start");
#endif    
}

void loop()
{
  switchVibration();
  
  //Joint battery manager
  Joint.battery();
  
  //watch dog trickle
  WTD.doggieTickle(); 
}

void switchVibration()
{   
  static unsigned long  timecnt = 0;
  
  if(digitalRead(SWITCH) == HIGH)
  {
     timecnt = millis();
  }
  
  if(millis() - timecnt >= TIMEUP)
  {
	  vibrate(1);
  }
  
}
void vibrate(float freq)
{
  int ms = (int)freq / 2.0 * 1000;
  digitalWrite(VIBRATOR, HIGH);
  delay(ms);
  digitalWrite(VIBRATOR, LOW);
  delay(ms);
}

