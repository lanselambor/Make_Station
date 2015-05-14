/*
* Coin counter.ino
* A demo for ChaiHuo ZaoWuBa Demo T14015
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
  static int i = 0;
  if(millis() - resetTime > TIMEOUTPERIOD){    
    Serial.println(++i);
    doggieTickle();                                          
	resetFunc();     
  }
  
}

//DeBug  switch 
#define  DeBug   0

//Libraries
#include <EEPROM.h>
#include "TM1637.h"
#define CLK 3      
#define DIO 5
TM1637 tm1637(CLK,DIO);
int16_t ListDisp[4];

const int linefinder =A5;
volatile  uint16_t num_money = 0;
int begin = 0;
int end = 0;

// =========  Setup  =========
void setup()
{
	pinMode (linefinder,INPUT);

    watchdogSetup();
	doggieTickle();
	
	tm1637.init();
	tm1637.set(BRIGHT_TYPICAL);	
	tm1637.clearDisplay();
	tm1637.display(3,0);
	
#if 0	// Don't erase eeprom when restarting
	if (EEPROM.read(3)!=0)
	{
		for(int i=0;i<10;i++)EEPROM.write(i,0);
		
	}
#endif	
	
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

}

// =========  Loop  =========
void loop()
{			


	doggieTickle();
         
        // Keep reading from eeprom ,then display data on 4-digital segment
	num_money = EEPROM.read(1);
	num_money = num_money << 8;	                   // Read form eeprom 
	num_money = num_money + EEPROM.read(0);	
	
	display_seg(num_money);   

        // If detect proximity
	if (digitalRead(linefinder)==LOW)                  // When detect proximity ,output LOW  
	{
		begin = millis();
		num_money ++;
		display_seg(num_money);                    // Display  		

#if DeBug  		
		Serial.println(num_money);
#endif 	
	    for(int i=0; i<2;i++)
		{
		        EEPROM.write(i,num_money & 0xff);        // Write into eeprom  
		        num_money = num_money >> 8;	
		}

	    while (digitalRead(linefinder) ==LOW)
		{
			
			doggieTickle();
			end = millis();
			if (end - begin >3000)
			{
				num_money = 0;
				for(int i=0; i<2;i++)
				{
					EEPROM.write(i,0);
				}					
				tm1637.clearDisplay();
				tm1637.display(3,0);
				//Serial.println("zero");
			}
		}
	}
}

void display_seg(uint16_t num_money)
{
	ListDisp[0] = num_money%10;
	ListDisp[1] = (num_money/10)%10;	
	ListDisp[2] = (num_money/100)%10;
	ListDisp[3] = (num_money/1000)%10;	
	/*
		tm1637.display(0,ListDisp[3]);
		tm1637.display(1,ListDisp[2]); 
		tm1637.display(2,ListDisp[1]);
		tm1637.display(3,ListDisp[0]);	
	
	*/
	if (ListDisp[3]!=0)
	{
		tm1637.display(0,ListDisp[3]);
		tm1637.display(1,ListDisp[2]); 
		tm1637.display(2,ListDisp[1]);
		tm1637.display(3,ListDisp[0]);			
	}
	
	if ((ListDisp[3]==0)&&(ListDisp[2]!=0))
	{
		tm1637.display(1,ListDisp[2]); 
		tm1637.display(2,ListDisp[1]);
		tm1637.display(3,ListDisp[0]);	
	}

	if ((ListDisp[3]==0)&&(ListDisp[2]==0)&&(ListDisp[1]!=0))
	{
		tm1637.display(2,ListDisp[1]);
		tm1637.display(3,ListDisp[0]);		
	}
        
	if ((ListDisp[3]==0)&&(ListDisp[2]==0)&&(ListDisp[1]==0)&&(ListDisp[0]!=0))
	{
		tm1637.display(3,ListDisp[0]);		
	}

}

