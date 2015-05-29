/*
* Moisture Led.ino
* A demo for ChaiHuo ZaoWuBa Demo T14014
* 
* Copyright (c) 2015 Seeed Technology Inc.
* Auther     : Jacob.Yan
* Create Time: Jan 07, 2015
* Change Log : Lambor.Fang update in May 2015
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
#include "VisualScope.h"
#include <Wire.h>

VisualScope VS;

//DeBug  switch 
#define  DeBug   0


//Libraries
#include <Adafruit_NeoPixel.h>
#define PIN            3
#define NUMPIXELS      20
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define lightness  3
#define sound_pin A5


int val_sound = 0;         
int quiet_value = 0;

// =========  Setup  =========
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

	//initial sound sensor
	long tmp=0, ave_num = 1000;
	
	for(int i=0;i<ave_num;i++)
	{
		tmp += analogRead(sound_pin);		
	}	
	quiet_value = tmp / ave_num;
	
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
    delay(10);
    val_sound = analogRead(sound_pin);   
	
#if DeBug 
	VS.Data_acquisition(val_sound,0,0,0);	
    //Serial.println(val_sound);  
#endif  	

    if (50 < (val_sound - quiet_value))action_rgbled_on ();  
	    else action_rgbled_off ();
	
	

}
void action_rgbled_on()
{
        //set different color data ,so that each time it display different color
	int i = 0;
	int j = 0;
	uint32_t time = millis();       
        uint8_t phase = (time >> 4) ;	                        
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
	
	for(int move_num = 0; move_num<NUMPIXELS+1; move_num++)     //"NUMPIXELS+1"    do not forget  +1
	{
		for(j = 0; j < move_num; j++)
		{
		        pixels.setPixelColor(j, pixels.Color(0,0,0)); // Moderately bright green color.
		        pixels.show(); // This sends the updated pixel color to the hardware.		
		}												
		for(i = move_num; i<NUMPIXELS; i++)
                {				
		        pixels.setPixelColor(i, pixels.Color(rgb_color[0],rgb_color[1],rgb_color[2])); // Moderately bright green color.
		        pixels.show(); // This sends the updated pixel color to the hardware.					
		}										
		delay(5);
	} 		
}

void action_rgbled_off ()
{
        for(int i=0;i<NUMPIXELS;i++)
	{
                // Moderately bright green color.
	        pixels.setPixelColor(i, pixels.Color(0,0,0)); 
                // This sends the updated pixel color to the hardware.	
	        pixels.show(); 	
        }	
}

