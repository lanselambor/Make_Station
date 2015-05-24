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

#define  DeBug  0

#define PWM_OUT_PIN       3
#define ULTRASONIC_PIN    A5
#define DEFAULT_DISTANCE  5
#define FILTER_ZONE       1

#define DISTANCE1         DEFAULT_DISTANCE * 1
#define DISTANCE2         DEFAULT_DISTANCE * 2
#define DISTANCE3         DEFAULT_DISTANCE * 3
#define DISTANCE4         DEFAULT_DISTANCE * 4
#define DISTANCE5         DEFAULT_DISTANCE * 5
#define DISTANCE6         DEFAULT_DISTANCE * 6
#define DISTANCE7         DEFAULT_DISTANCE * 7
#define DISTANCE8         DEFAULT_DISTANCE * 8
#define DISTANCE9         DEFAULT_DISTANCE * 9

Ultrasonic ultrasonicAir(ULTRASONIC_PIN);
unsigned int noteDurations = 50 ;
boolean playBack = false;


void setup()
{ 

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
	//while(1);   //debug  watchdog 	
#if DeBug	
    Serial.begin(9600);
	Serial.println("start");
#endif  	

}

void loop()
{
    volatile unsigned char airGamutValue = 0;
    volatile unsigned int ultrasonicAirDistance = 0;
	
    WTD.doggieTickle(); 
	
	
    while(1)
    {                
        ultrasonicAirDistance = ultrasonicAir.MeasureInCentimeters(); 
        
        if     ((ultrasonicAirDistance>DISTANCE1)&&(ultrasonicAirDistance<=DISTANCE2))airGamutValue = 1, playBack = true;
        else if((ultrasonicAirDistance>DISTANCE2)&&(ultrasonicAirDistance<=DISTANCE3))airGamutValue = 2, playBack = true;
        else if((ultrasonicAirDistance>DISTANCE3)&&(ultrasonicAirDistance<=DISTANCE4))airGamutValue = 3, playBack = true;
        else if((ultrasonicAirDistance>DISTANCE4)&&(ultrasonicAirDistance<=DISTANCE5))airGamutValue = 4, playBack = true;
        else if((ultrasonicAirDistance>DISTANCE5)&&(ultrasonicAirDistance<=DISTANCE6))airGamutValue = 5, playBack = true;
        else if((ultrasonicAirDistance>DISTANCE6)&&(ultrasonicAirDistance<=DISTANCE7))airGamutValue = 6, playBack = true;
        else if((ultrasonicAirDistance>DISTANCE7)&&(ultrasonicAirDistance<=DISTANCE8))airGamutValue = 7, playBack = true;
        else if((ultrasonicAirDistance>DISTANCE8)&&(ultrasonicAirDistance<=DISTANCE9))airGamutValue = 8, playBack = true;
        else airGamutValue = 0, playBack = false;
		
		Gamut_Play(airGamutValue);
#if DeBug
		Serial.print("Ult = ");
        Serial.println(ultrasonicAirDistance);
#endif		    				
		
#if DeBug             
		Serial.print("Air Gamut Value is:"); 
		Serial.println(airGamutValue); 
#endif             	
			
        delay(10);       
		WTD.doggieTickle();   
    }
}

void Gamut_Play(unsigned char data)
{
    if(playBack)
	{
		switch(data)
		{      
			case 1:
				tone(PWM_OUT_PIN,523);//Do(523Hz)NOTE_C5
				break;        
			case 2:
				tone(PWM_OUT_PIN,587);//Re(587Hz)NOTE_D5
				break;        
			case 3:
				tone(PWM_OUT_PIN,659);//Mi(659Hz)NOTE_E5
				break;        
			case 4:
				tone(PWM_OUT_PIN,698);//Fa(698Hz)NOTE_F5
				break;        
			case 5:
				tone(PWM_OUT_PIN,784);//So(784Hz)NOTE_G5
				break;        
			case 6:
				tone(PWM_OUT_PIN,880);//La(880Hz)NOTE_A5
				break;       
			case 7:
				tone(PWM_OUT_PIN,988);//Si(988Hz)NOTE_B5
				break;       
			case 8:
				tone(PWM_OUT_PIN,1047);//Do(1047Hz)NOTE_C6
				break;             
			default:
				noTone(PWM_OUT_PIN);
				break;         
		}
	}
	else 
	{
		noTone(PWM_OUT_PIN);
	}
	
}



