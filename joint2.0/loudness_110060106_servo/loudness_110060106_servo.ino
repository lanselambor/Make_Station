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
#include "WatchDog.h"
#include "VisualScope.h"
#include <Wire.h>
#include <Servo.h>
 
#define  DEBUG   0

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

#if DEBUG
VisualScope vs;
#endif



#define original_pos 90 
// Servo position begin value
#define pos_begin  80
// Servo position end value
#define pos_end    100
// Create servo object to control a servo
Servo myservo;

// Sound sensor pin input define
const int pin_sound = A5;

//sound analog value
int quiet_value = 0; 
int val_sound = 0;

//value off the thershold, 
int thershold_off = 50;
//sound threshold
int Threshold[5] = {40 + thershold_off, 
                    60 + thershold_off, 
                    80 + thershold_off,
                    100 + thershold_off, 
                    120 + thershold_off};
   

const long interval = 50;
unsigned long previousMillis = 0;
unsigned long currentMillis  = 0;

int midNum(int *a, int *b, int *c);
int average_filter(int analog_pin, int num);
int mid_filter(int analog_pin);
void delay_feed( int val);

void setup()
{ 
  //power up
  pinMode(CHRG_LED, OUTPUT);
  digitalWrite(CHRG_LED, LOW);
  
  //initial watchdog  
  WTD.watchdogSetup();
  WTD.doggieTickle();

  //get quiet sound value
  delay(500);
  long tmp = 0;
  for(int i = 0;i<1000;i++)
  {
    tmp += analogRead(pin_sound);
  }
  quiet_value = tmp / 1000;

  pinMode (10,OUTPUT);
  for(int i=0;i<2;i++)
  {
    analogWrite(10,5);
    delay(500);
    analogWrite(10,0);     
    delay(500);  
    WTD.doggieTickle();
  } 

  #if DEBUG  
  Serial.begin(9600);
  Serial.println("start");
  #endif    

  myservo.attach(3);  //    mini fan use pin 9   joint use pin 3
  previousMillis = millis();
  myservo.write(90); 

}

void loop()
{  
  WTD.doggieTickle();                 
  val_sound = average_filter(pin_sound, 50);       
  int threshold = val_sound - quiet_value;
  if(threshold > thershold_off)
  {
    Serial.println(threshold);
    servoRun(threshold);
  }              
  WTD.doggieTickle();   
  delay(200);  
}

void servoRun(int analog)
{
  int dec = 0;
  if(analog < 200)
  {  
    dec = map(analog, thershold_off, 200, 2, 30);
  }
  else
  {
    dec = 30;
  }
  myservo.write(original_pos - dec);
  delay(200);
  myservo.write(original_pos + dec);
  delay(200);
}
// Delay with feed dog
void delay_feed( int val)
{
  delay(val);
  WTD.doggieTickle(); 
}

int mid_filter(int analog_pin)
{
  int a = analogRead(analog_pin);
  delayMicroseconds(10);
  int b = analogRead(analog_pin);
  delayMicroseconds(10);
  int c = analogRead(analog_pin);
  delayMicroseconds(10);
  
  return midNum(&a, &b, &c);
}

int average_filter(int analog_pin, int num)
{
  long temp = 0;
  for(int i=0;i<num;i++)
  {
    //temp += analogRead(analog_pin);    
    temp += mid_filter(analog_pin);    
  }
  return temp/num;
}

int midNum(int *a, int *b, int *c)
{
  int tmp = 0;
  if(*a > *b){
    tmp = *a;
    *a = *b;
    *b = tmp;
  }
  if(*b > *c){
    tmp = *b;
    *b = *c;
    *c = tmp;
  }
  if(*b > *c){
    tmp = *b;
    *b = *c;
    *c = tmp;
  }
  return *b;
}
