/*
* Moisture Led.ino
* A demo for ChaiHuo ZaoWuBa Demo T14014
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

#define  ENABLE_WTD  0

unsigned long resetTime = 0;
volatile bool  flg_power = 0;
void(* resetFunc) (void) = 0;  

#if ENABLE_WTD
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
#endif


//DeBug  switch 
#define  DeBug   0

#include <Adafruit_NeoPixel.h>
#define PIN            3
#define NUMPIXELS      1
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


const int analogPin_in = A5;   
int val_sound = 0;         

#define lightness  2

// =========  Setup  =========
void setup()
{

	
        // This initializes the NeoPixel library.
	pixels.begin(); 
//==============================//

#if ENABLE_WTD
	watchdogSetup();
#endif
    
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
}

// =========  Loop  =========
void loop()
{
    

    
#if ENABLE_WTD
	doggieTickle();
#endif
    
	uint16_t val = analogRead(analogPin_in);   
#if DeBug	
	Serial.println(val);
#endif  	
	//uint8_t val_ =  map(val, 0, 1023, 0, 80);	
	uint8_t phase =  map(val, 0, 700, 1, 80);	
#if DeBug	
	Serial.println(phase);
#endif          
	float h = phase / 256.0;
	float s = 1.0;
	float v = 1.0;

	int i_ = floor(h * 6);
	float f = h * 6 - i_;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);
	float r = 0, g = 0, b = 0;
        
	switch(i_ % 6)
        {
		case 0: r = v; g = t; b = p; break;
		case 1: r = q; g = v; b = p; break;
		case 2: r = p; g = v; b = t; break;
		case 3: r = p; g = q; b = v; break;
		case 4: r = t; g = p; b = v; break;
		case 5: r = v; g = p; b = q; break;
        }	
        		
	uint8_t  rgb_color[3] ;
	rgb_color[0] = r * 255;
	rgb_color[1] = g * 255;	
	rgb_color[2] = b * 255;	
	
	rgb_color[0] = rgb_color[0] >>lightness;
	rgb_color[1] = rgb_color[1] >>lightness;	
	rgb_color[2] = rgb_color[2] >>lightness;	
			
        for(int i=0;i<NUMPIXELS;i++)
        {
                // Moderately bright green color.
		pixels.setPixelColor(i, pixels.Color(rgb_color[0],rgb_color[1],rgb_color[2])); 
                // This sends the updated pixel color to the hardware.
		pixels.show(); 		
	}	

}


