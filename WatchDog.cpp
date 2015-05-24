/*
* WatchDog.cpp
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
#include <Arduino.h>

WatchDog WTD(2000);

ISR(WDT_vect) 
{ 	
	if(millis() - WTD.ResetTime > WTD._timeout)
	{    
		WTD.doggieTickle();                                          
		WTD.resetFunc();     
	}  
}

WatchDog::WatchDog(long timeout)
{
	_timeout = timeout;
}

void WatchDog::watchdogSetup()
{
	cli();  
	wdt_reset(); 
	MCUSR &= ~(1<<WDRF);  
	WDTCSR = (1<<WDCE) | (1<<WDE);
	WDTCSR = (1<<WDIE) | (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (0<<WDP0);
	sei();
}

void WatchDog::doggieTickle(void)
{
	ResetTime = millis();
}



