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
#include "WatchDog.h"
#include "Ultrasonic.h"
#include "TimerOne.h"
#include "SoftwareSerial.h"

#define DeBug  1

#define MAN_LEAVE 10  //nothing close inside 10cm

int vibration_pin = 3;
int ultrasonic_pin = A5;
volatile uint16_t distance = 0;
volatile long timing = 0;


Ultrasonic ultrasonicAir(ultrasonic_pin);
unsigned int noteDurations = 50 ;
boolean playBack = false;


void setup()
{ 
	WTD.watchdogSetup();
	WTD.doggieTickle();
	
	pinMode(vibration_pin,OUTPUT);
	pinMode(ultrasonic_pin,INPUT);
	
	pinMode (10,OUTPUT);
	for(int i=0;i<2;i++)
	{
		digitalWrite(10,HIGH);
		delay(500);
		digitalWrite(10,LOW); 		
		delay(500);	
		WTD.doggieTickle();
	}
	
	Timer1.initialize(50000);//timing for 50ms
	Timer1.attachInterrupt(TimingISR);//declare the interrupt serve routine:TimingISR  
	
#if DeBug	
    Serial.begin(9600);
	Serial.println("start");
#endif  	

}

void TimingISR(void)
{   
	distance = ultrasonicAir.MeasureInCentimeters(); 
	timing += 50;
}

void vibration()
{
	digitalWrite(vibration_pin, HIGH);
	delay(500);
	digitalWrite(vibration_pin, LOW);
	delay(500);
}
void loop()
{                   

	WTD.doggieTickle();
	if(distance > MAN_LEAVE)   //No Man sitting in
	{
		timing = 0;
	}
	else if((timing >= 5000) && (distance <= MAN_LEAVE))  //Man sitting in
	{
		timing = 5000;
		vibration();
		WTD.doggieTickle();
	}
		
	
#if DeBug
	Serial.print("Ult = ");
	Serial.println(distance);
#endif		    						
	delay(100);       
	WTD.doggieTickle();   
    
}