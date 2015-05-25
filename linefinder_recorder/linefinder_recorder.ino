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

#define  DeBug   0

const int linefinder =A5;
const int recorder = 3;
const int time_to_drink = 5000;

void setup()
{		   
	WTD.watchdogSetup();
	WTD.doggieTickle();
	
	pinMode(linefinder,INPUT);    
	pinMode(recorder, OUTPUT);
	digitalWrite(recorder, LOW);
	
	pinMode (10,OUTPUT);
	for(int i=0;i<2;i++)
	{
        digitalWrite(10,HIGH);
        delay(500);
        digitalWrite(10,LOW); 		
        delay(500);	
        WTD.doggieTickle();
	}	
	
#if DeBug	
    Serial.begin(9600);
	Serial.println("start");
#endif  	

}

void speak()
{
	digitalWrite(recorder, HIGH);
	delay(1000);
	digitalWrite(recorder, LOW);
	WTD.doggieTickle(); 
	delay(1000);
	digitalWrite(recorder, HIGH);
	WTD.doggieTickle(); 
	delay(1000);
	digitalWrite(recorder, LOW);
	WTD.doggieTickle(); 
	delay(1000);
}
void loop()
{			
	volatile long timing = millis();
	
	while(!digitalRead(linefinder))
	{
		if( (millis() - timing) > time_to_drink )
		{
			speak();  			
			timing = millis();
		}
		
		WTD.doggieTickle();           
	}
	delay(100);
	WTD.doggieTickle();           
}
