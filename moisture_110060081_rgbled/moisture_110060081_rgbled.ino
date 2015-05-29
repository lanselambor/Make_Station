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

#include "WatchDog.h"
#include <Wire.h>


//DeBug  switch 
#define  DeBug   0

#include <Adafruit_NeoPixel.h>
#define PIN            3
#define NUMPIXELS      1
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


const int analogPin_in = A5;   
int val_sound = 0;         

#define lightness  2

void setup()
{	
	//power up
	pinMode(6, OUTPUT);
	digitalWrite(6, LOW);	
	
	WTD.watchdogSetup();
	WTD.doggieTickle();
	
	pixels.begin(); 
    
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
//==============================//	
}

// =========  Loop  =========
void loop()
{
	WTD.doggieTickle();
    
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


