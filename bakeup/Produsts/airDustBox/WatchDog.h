/*
* WatchDog.h
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
#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#include <avr/wdt.h>

class WatchDog
{
	public:
		//initial watchdog timeout
		WatchDog(long timeout = 2000);		
		
		//method
		void watchdogSetup(void);
		void doggieTickle(void);		
		void(* resetFunc) (void) = 0;  		
		
		//parameters
		unsigned long ResetTime;
		volatile bool  Flg_Power;
		long _timeout;
		
};
extern WatchDog WTD;
#endif