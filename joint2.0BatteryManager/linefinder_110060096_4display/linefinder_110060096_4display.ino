/*
* Coin counter.ino
* A demo for ChaiHuo ZaoWuBa Demo T14015
*
* Copyright (c) 2015 Seeed Technology Inc.
* Auther     : Jacob.Yan
* Create Time: Jan 07, 2015
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
#include <Wire.h>
#include <EEPROM.h>
#include "TM1637.h"
#include <Arduino.h>
#include "GroveJoint.h"

#define DeBug 0

uint16_t coinNumber = 0;
TM1637 coinCounterDisplay(3, 5);  //clk = 3, dio = 5;

void zeroCoinNumberOnEeprom();
void writeCoinNumberToEeprom(uint16_t num);
uint16_t readCoinNumberFromEeprom(void);
void coinCounterInit();
void displayCoinNumber(uint16_t num);

// =========  Setup  =========
void setup()
{
  //Joint hardware init
  Joint.initHardware();

  //Init watch dog
  WTD.watchdogSetup();
  WTD.doggieTickle();

  coinCounterInit();
/*
  pinMode (10,OUTPUT);
  for(int i=0;i<2;i++)
  {
        analogWrite(10,5);
        delay(500);
        analogWrite(10,0);
        delay(500);
        WTD.doggieTickle();
  }
*/
  //while(1);   //debug  watchdog
#if DeBug
  Serial.begin(9600);
  Serial.println("start");
#endif    
}

// =========  Loop  =========
void loop()
{
  WTD.doggieTickle();
  Joint.battery();
  coinCounter();
}

void coinCounter()
{
  int lineFiner = A5;
  coinNumber = readCoinNumberFromEeprom();

  if(digitalRead(lineFiner) == LOW)
  {
    long begin = 0;

    begin = millis();
    coinNumber++;
    writeCoinNumberToEeprom(coinNumber);
    displayCoinNumber(coinNumber);

    while(digitalRead(lineFiner) == LOW)
    {
      if(millis() - begin >= 3000)
      {
        coinNumber = 0;
        begin = millis();
        zeroCoinNumberOnEeprom();
        displayCoinNumber(coinNumber);
      }
	  displayCoinNumber(coinNumber);
	  Joint.battery();  //Bateery manager
	  WTD.doggieTickle();
      //delay(10);
    }
    displayCoinNumber(coinNumber);
  }
  writeCoinNumberToEeprom(coinNumber);
  displayCoinNumber(coinNumber);
  //delay(10);
}

void displayCoinNumber(uint16_t num)
{
  uint8_t thousand = num/1000;
  uint8_t hundred = num%1000/100;
  uint8_t tens = num%100/10;
  uint8_t unit = num%10;

  coinCounterDisplay.clearDisplay();
  if(thousand != 0)
  {
    coinCounterDisplay.display(0,thousand);
  }
  if((thousand != 0) || (hundred != 0))
  {
    coinCounterDisplay.display(1,hundred);
  }
  if((thousand != 0) || (hundred != 0) || (tens != 0))
  {
    coinCounterDisplay.display(2,tens);
  }
  coinCounterDisplay.display(3,unit);
}

void coinCounterInit()
{
  int lineFiner = A5;
  pinMode(lineFiner, INPUT);
  digitalWrite(lineFiner, HIGH);

  coinCounterDisplay.init();
  coinCounterDisplay.set(BRIGHT_TYPICAL);
  coinCounterDisplay.clearDisplay();
  coinCounterDisplay.display(3,0);
}
uint16_t readCoinNumberFromEeprom(void)
{
  return (uint16_t)( EEPROM.read(1) + EEPROM.read(2) * 256 );
}

void writeCoinNumberToEeprom(uint16_t num)
{
  EEPROM.write(1, num%256);
  EEPROM.write(2, num/256);
}

void zeroCoinNumberOnEeprom()
{
  EEPROM.write(1, 0);
  EEPROM.write(2, 0);
}
