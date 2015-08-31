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
#define THRESHOLD 100

int sound_pin = A5;
int motor = 3;
volatile int sound_value = 0;
volatile int last_value = 0;
volatile bool Motor_Sleep = false;
volatile bool motor_run_flag = false;

void setup() 
{ 
  Joint.initHardware();
  //initial WatchDog
  WTD.watchdogSetup();
  WTD.doggieTickle();   
  
  pinMode(sound_pin, INPUT);
  pinMode(motor,OUTPUT);
   
  Timer1.initialize(50000);//timing for 50ms
  Timer1.attachInterrupt(TimingISR);//declare the interrupt serve routine:TimingISR  
  
#if DEBUG     
  Serial.begin(9600);
  Serial.println('start');
#endif  
}

void TimingISR(void)
{   
  WTD.doggieTickle();
  sound_value = analogRead(sound_pin);
  if(THRESHOLD < (sound_value - last_value))
  {
    motor_run_flag = true;
  }
  last_value = sound_value;
}

void loop() 
{	
  if(motor_run_flag)
  {
    motorRun();
  }
  WTD.doggieTickle();
  
  //Joint battery manager
  Joint.battery();  
  delay(500);
}

void motorRun()
{
  motor_run_flag = false;
  WTD.doggieTickle();
  for(int i=0; i<100; i++)
  {
    analogWrite(motor, 20);
    delay(10);
    digitalWrite(motor, 0);  
    delay(10);
	
	//Joint battery manager
    Joint.battery();
    WTD.doggieTickle();	
  }
}
