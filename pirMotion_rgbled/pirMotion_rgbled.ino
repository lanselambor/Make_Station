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

#define DEBUG 0

#define LIGHT_UP_TIME 4000//10*60*1000

int led = 3;  //led control output pin
int pir = A5;   //PIR_Sensor input pin

void setup() {
	WTD.watchdogSetup();
	WTD.doggieTickle();
	
	pinMode (10,OUTPUT);
	for(int i=0;i<2;i++)
	{
		digitalWrite(10,HIGH);
		delay(500);
		digitalWrite(10,LOW); 		
		delay(500);	
		WTD.doggieTickle();
	}
	
	Serial.begin(9600);
	pinMode(led, OUTPUT);
	pinMode(pir, INPUT);
  
}

void loop() {
	if(digitalRead(pir))
	{
		unsigned long begin = millis();

		for(int i=255; i >= 0; i--)
		{
			long y = cal_circle_y(i, 255); 
#if DEBUG			
			Serial.println(y);    
#endif			
			analogWrite(led,y);
			delay(10);      
		}    

		while(LIGHT_UP_TIME > millis() - begin)
		{
			if(digitalRead(pir))
			{
				begin = millis();
			}
		}
	} 
	
	//Turn off the light
	for(int i = 0; i<=255; i++)
	{
		long y = cal_circle_y(i, 255);  
		
#if DEBUG					
		Serial.println(y);    
#endif		
		analogWrite(led,y);
		delay(10);      
	}    

	while(!digitalRead(pir))
	{
		long static counter = 0;
		delay(1);
		counter ++;
		
		if(500 <= counter) {
			WTD.doggieTickle();		
			counter = 0;
		}
	};
	
	WTD.doggieTickle();
}

//r * r = (x-r)*(x-r) + y * y 
long cal_circle_y(long x, long r)
{
    return sqrt((r * r) - (x - r) * (x - r));
}