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
#include <Arduino.h>
#include "WatchDog.h"
#include <TimerOne.h>
#include <Wire.h>
#include "GroveJoint.h"

#define  DeBug   0

#define LINE_FINDER_FIND       (LOW == digitalRead(A5))   //something cover the sensor
#define LINE_FINDER_NOTFIND    (HIGH == digitalRead(A5)) //nothing cover the sensor
#define TIME_TO_DRINK          2700000   //unit: Ms. 2700000 Ms = 45 Min, time to drink water
#define SPEAK_DURATION         4         //4 s

const int linefinder  = A5;
const int recorder    = 3;

void setup()
{
	//initial watchdog
	WTD.watchdogSetup();
	WTD.doggieTickle();

	Joint.initHardware();
	cupWadInit();

#if DeBug
	Serial.begin(9600);
	Serial.println("start");
#endif    
}

void loop()
{
	WTD.doggieTickle();
	Joint.battery();
	cupWad();
}

void cupWad()
{
	static volatile unsigned long clock = 0;
	static volatile bool readyTiming = true;

	if (LINE_FINDER_FIND)
	{
		if(readyTiming)
		{
			readyTiming = false;
			speak(1, SPEAK_DURATION);
			clock = millis();
		}
	}
	else if(LINE_FINDER_NOTFIND)
	{
		readyTiming = true;
		clock = millis();
	}

	if( millis() - clock > TIME_TO_DRINK )
	{
		speak(3, SPEAK_DURATION);
		clock = millis();
	}
}
void cupWadInit()
{
  //initial devices
  pinMode(linefinder,INPUT);
  pinMode(recorder, OUTPUT);
  digitalWrite(recorder, LOW);
}

void speak(int times, int seconds)
{
  for(int i=0; i<times; i++)
  {
	digitalWrite(recorder, HIGH);
	delaySeconds(seconds);
	digitalWrite(recorder, LOW);
	delaySeconds(1);
	if(LINE_FINDER_NOTFIND)
	{
	  i = times;
	}
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
