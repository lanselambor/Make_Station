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
#include <TimerOne.h>
#include "WatchDog.h"
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include "GroveJoint.h"

#define DEBUG 0   //enable debug

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

#define MOTOR_SPEED 5
#define THRESHOLD 30

#define NUMPIXELS      20
#define LIGHT_PIN      3
#define PIN            LIGHT_PIN

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int sound_pin = A5;
int motor = 3;
volatile int sound_value = 0;
volatile int last_value = 0;
volatile bool Motor_Sleep = false;


void setup() 
{ 
  //Joint hardware init
  Joint.initHardware();
  
  //initial WatchDog
  WTD.watchdogSetup();
  WTD.doggieTickle();  
  
  pinMode(sound_pin, INPUT);
  pinMode(motor,OUTPUT);
 
#if DEBUG   
  Serial.begin(9600);
  Serial.println("start");
#endif
}

void loop() 
{
  //Jiont battery manager 
  Joint.battery();
  
  sound_value = analogRead(sound_pin);
  if(40 < (sound_value - last_value))
  {
    WTD.doggieTickle();
    action_rgbled_on ();    
    action_rgbled_off();
  }
  last_value = sound_value;
  WTD.doggieTickle();
  delay(50);
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
  
  rgb_color[0] = rgb_color[0] >>LIGHT_PIN;
  rgb_color[1] = rgb_color[1] >>LIGHT_PIN;  
  rgb_color[2] = rgb_color[2] >>LIGHT_PIN;    
  
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
    delay(1);
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



