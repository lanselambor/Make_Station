/*
* Groot.ino
* A demo for ChaiHuo ZaoWuBa Demo T14006
* 
* Copyright (c) 2015 Seeed Technology Inc.
* Auther     : Jacob.Yan
* Create Time: Jan 07, 2015
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



//Watchdog setting
#include <avr/wdt.h>
#define doggieTickle() resetTime = millis()
#define TIMEOUTPERIOD 2000
unsigned long resetTime = 0;
volatile bool  flg_power = 0;
void(* resetFunc) (void) = 0;  
void watchdogSetup()
{
    cli();  
    wdt_reset(); 
    MCUSR &= ~(1<<WDRF);  
    WDTCSR = (1<<WDCE) | (1<<WDE);
    WDTCSR = (1<<WDIE) | (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (0<<WDP0);
    sei();
}
ISR(WDT_vect) 
{ 
  if(millis() - resetTime > TIMEOUTPERIOD){
    doggieTickle();                                          
	resetFunc();     
  }
}

//DeBug  switch 
#define  DeBug   0

#include <Servo.h> 
// Servo position begin value
#define pos_begin  80
// Servo position end value
#define pos_end    100

// Sound sensor pin input define
const int pin_sound = A5;

// Create servo object to control a servo
Servo myservo;   

// Analog pin used to connect the potentiometer
int potpin = A5;  

// Variable to read the value from the analog pin 
int val;    

const long interval = 50;
unsigned long previousMillis = 0;
unsigned long currentMillis  = 0;

#define  interval_val   100

int table[9]= {1,2,3,4,5,6,7,8,9};

// =========  Setup  =========
void setup()
{
//==============================//
	watchdogSetup();
	pinMode (10,OUTPUT);
	for(int i=0;i<2;i++)
	{
	digitalWrite(10,HIGH);
	delay(500);
	digitalWrite(10,LOW); 		
	delay(500);	
	doggieTickle();
	}
	//while(1);   //debug  watchdog 	
#if DeBug	
    Serial.begin(9600);
	Serial.println("start");
#endif  	
//==============================//	


	myservo.attach(3);  //    mini fan use pin 9   joint use pin 3
	previousMillis = millis();
	myservo.write(90); 
}

// =========  Loop  =========
void loop()
{	
	currentMillis = millis();
	if(currentMillis - previousMillis > interval)
 	{	
	    doggieTickle();     

		int val_sound = (analogRead(pin_sound));
		Serial.println (val_sound);

		if ((val_sound > 150)&&(val_sound < 200))
		{		 		
			myservo.write(pos_begin - 10);   
			delay_feed(5*10);
			myservo.write(pos_end + 10);  	
			delay_feed(5*10);
		}	
                
		if ((val_sound > 200)&&(val_sound < 300))
		{		 
			myservo.write(pos_begin);   
			delay_feed(5*20);
			myservo.write(pos_end);  	
			delay_feed(5*20);
		}	
                
		if ((val_sound > 300)&&(val_sound < 350))
		{		 		
			myservo.write(pos_begin - 10);   
			delay_feed(5*40);
			myservo.write(pos_end + 10);  	
			delay_feed(5*40);
		}
                	
		if ((val_sound > 350)&&(val_sound < 400))
		{		 	
			myservo.write(pos_begin - 20);   
			delay_feed(5*60);
			myservo.write(pos_end + 20);  	
			delay_feed(5*60);
		}
                	
		if ((val_sound > 400)&&(val_sound < 450))
		{		 		
			myservo.write(pos_begin - 30);   
			delay_feed(5*80);
			myservo.write(pos_end + 30);  	
			delay_feed(5*80);
		}
		previousMillis = millis();
	
	delay(80);
	doggieTickle(); 
	}	
}

// Delay with feed dog
void delay_feed( int val)
{
       delay(val);
	   doggieTickle(); 

}







