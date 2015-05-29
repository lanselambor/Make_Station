#include "WatchDog.h"

#include <EEPROM.h>
#include <TimerOne.h>
#include <TM1637.h>
#include <IRSendRev.h>

#define KEY_PUSHED  digitalRead(button)==HIGH
//board 
const int button = 2;
const int led = 10;
const int pwr = 6;
const int pwr_hold = A1;
const int lightness =A0;
const int batled =A3;
bool state_ = 0;

// mode val
volatile unsigned int val=0;
unsigned int val_detecting=0;
volatile unsigned int function_num=0;
unsigned int par_num=0;

#define    POWER_OFF            1
#define    POWER_ON             2
#define    SET_MODE_FUNCTION    3
#define    SET_MODE_PAR         4
#define    SET_MODE_ALL         5
#define NIGHT       analogRead(lightness)<15
#define DAY         analogRead(lightness)>900


volatile bool  flg = 0;
//volatile bool  flg_power = 0;
volatile unsigned int  flg_val = 0;
int state = 1;
volatile int mode = POWER_ON;
int lightness_begin = 0;

//B-SEGMENT
#define CLK 3     
#define DIO 5
TM1637 tm1637(CLK,DIO);
int16_t ListDisp[4];

//IR-D
unsigned char dta[20];
int ir_pwm_val = 0; 
bool ir_switch = 0;
const int BassTab[]={1023,956,851,758,716,637,568,506,460};//bass 1~7

//Timing_B
unsigned long halfsecond = 0;
unsigned long second = 0;
unsigned long minute = 0;
unsigned long hour = 12;
int second_countdown = 10;
unsigned long start_time_timing = 0;
unsigned long end_time_timing   = 0;
long duration_timing   = 0;

//B_rgbled
/**/
#include <Adafruit_NeoPixel.h>
#define PIN            3
#define NUMPIXELS      20
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define lightness_rgbled  3

//Groot
#include <Servo.h> 
#define pos_begin  80
#define pos_end    100

const int pin_sound = A5;
Servo myservo;   
const long interval = 50;
unsigned long previousMillis = 0;
unsigned long currentMillis  = 0;


//air  piano
#include <Ultrasonic.h>
#define PWM_OUT_PIN       3
#define ULTRASONIC_PIN    A5
#define DEFAULT_DISTANCE  5
#define DISTANCE1         DEFAULT_DISTANCE * 1
#define DISTANCE2         DEFAULT_DISTANCE * 2
#define DISTANCE3         DEFAULT_DISTANCE * 3
#define DISTANCE4         DEFAULT_DISTANCE * 4
#define DISTANCE5         DEFAULT_DISTANCE * 5
#define DISTANCE6         DEFAULT_DISTANCE * 6
#define DISTANCE7         DEFAULT_DISTANCE * 7
#define DISTANCE8         DEFAULT_DISTANCE * 8
#define DISTANCE9         DEFAULT_DISTANCE * 9

Ultrasonic ultrasonicAir(ULTRASONIC_PIN);

//color sensor
//#include <Adafruit_NeoPixel.h>
#include <TCS3414CS.h>
#include <Wire.h>

void setup() 
{                
	
//-------------board initialize----------------// 
	if(EEPROM.read(5)!=1)
	{
		for (int i=0;i<10;i++)
		{
			EEPROM.write(i,1);
		}
		
	}	
	
	function_num     =  EEPROM.read(0);                      //get "X"
	par_num          =  EEPROM.read(function_num);           //get "Y"

	pinMode(led, OUTPUT);                                    //initialize led and display x,y 
	digitalWrite(led, LOW);
	blink_led(function_num,250);                             
	delay_feed(1500);
	blink_led(EEPROM.read(function_num),250);	
	
	
	pinMode(pwr, OUTPUT);                                  //initialize pwrled and batled    
	digitalWrite(pwr, HIGH);	
	pinMode(batled, OUTPUT);  
	digitalWrite(batled, HIGH);
	
	pinMode(button, INPUT);	                              //initialize button and enable software power switch   
	pinMode(pwr_hold,OUTPUT);
	digitalWrite(pwr_hold,LOW); 
	

	digitalWrite(batled, LOW);
		
	//Serial.begin(9600);
	//Serial.print("start        ");
	//Serial.println(lightness_begin);	
	
	
//--------------------start interrupt----------------//	
	attachInterrupt(0, start_timing, RISING );	
	
	WTD.watchdogSetup();
	WTD.doggieTickle(); 
	
}

void loop() {
	WTD.doggieTickle(); 

//---------------------------mode switch-----------//	
if(flg)
	{ 
		detachInterrupt(0);
		delay_feed(20);
		unsigned long pushTime_begin = millis();
		while(KEY_PUSHED)
			{	delay_feed(20);
				if (millis() - pushTime_begin > 3000) analogWrite(pwr,254);				
				if ((millis() - pushTime_begin > 1500)&&(millis() - pushTime_begin < 3000) )analogWrite(pwr,240);		
				if ((millis() - pushTime_begin > 500)&&(millis() - pushTime_begin < 1500) )analogWrite(pwr,1);				
			}
		unsigned long interval_time = millis() - pushTime_begin;
		if (interval_time > 3000) 
			{
			mode = SET_MODE_ALL;
			}
		if ((interval_time > 1500)&&(interval_time < 3000) )
			{
			mode = SET_MODE_PAR;
			}	
			if ((interval_time > 500)&&(interval_time < 1500) )
			{
			mode = SET_MODE_FUNCTION;
			}			
		if (interval_time <500 )
			{ 
			state++;
			if (state==2)state=0;
			//analogWrite(led,200*state);
			if (state==0)mode = POWER_OFF;
				else mode = POWER_ON;
			}		

		delay_feed(50);
		attachInterrupt(0, start_timing, RISING );
		flg = 0;
	}
	

	switch (mode){ 		
		case POWER_OFF ://Serial.println("POWER_OFF");	
			poweroff();
			break;			
		case POWER_ON ://Serial.println("POWER_ON");	
			POWERON();	
			break;	
		case SET_MODE_FUNCTION : //Serial.println("SET_MODE_FUNCTION");	
			SETMODE_FUNCTION();	
			break;			
				
		case SET_MODE_PAR ://Serial.println("SET_MODE_PAR");	
			SETMODE_PAR();	
			break;	
			
		case SET_MODE_ALL ://Serial.println("SET_MODE_PAR");	
			SETMODE_ALL();	
			break;				
				
		}			
	
}

void SETMODE_FUNCTION()//SET X
{    //cli(); 
	WTD.doggieTickle(); 
	digitalWrite(led ,LOW);
	Timer1.detachInterrupt();	
	Timer1.initialize(10000);
	Timer1.attachInterrupt(Time1_flg);
	//detachInterrupt(0);
	pinMode(button, INPUT);
	int enviroment_lightness = 0;
	

	enviroment_lightness     = analogRead(A0);
	
	
	while(!flg)
	{
		WTD.doggieTickle(); 	
		
		if (NIGHT)
			{
				delay_feed(20);
				val=0;	
				while(NIGHT) WTD.doggieTickle(); 
				val_detecting=val;
				val=0;
				
				if (val_detecting>5)	
					{
					function_num++;
					if (function_num==5)function_num=1;
					EEPROM.write(0,function_num);
					blink_led(function_num,250);		
					}				
			}
		
		
		if (DAY)
			{
				delay_feed(20);
				val=0;	
				while(DAY) WTD.doggieTickle();
				val_detecting=val;
				val=0;
				
				if (val_detecting>5)	
					{
					function_num++;
					if (function_num==5)function_num=1;
					EEPROM.write(0,function_num);
					blink_led(function_num,250);		
					}	
			}	
			
		enviroment_lightness     = analogRead(A0);


		
			
	}
	
	detachInterrupt(0);
	analogWrite(pwr,250);
	delay_feed(50);	
	while(digitalRead(button)==HIGH) WTD.doggieTickle();
	delay_feed(50);	
	attachInterrupt(0, start_timing, RISING );
	
	
	while(flg)
	{
			
		WTD.doggieTickle();
		if (NIGHT)
			{
				delay_feed(20);
				val=0;	
				while(NIGHT) WTD.doggieTickle();
				val_detecting=val;
				val=0;
				
				if (val_detecting>5)	
					{
					par_num++;
					if (par_num==5)par_num=1;
					EEPROM.write(function_num,par_num);
					blink_led(par_num,250);		
					}				
			}
		
		
		if (DAY)
			{
				delay_feed(20);
				val=0;	
				while(DAY) WTD.doggieTickle();
				val_detecting=val;
				val=0;
				
				if (val_detecting>5)	
					{
					par_num++;
					if (par_num==5)par_num=1;
					EEPROM.write(function_num,par_num);
					blink_led(par_num,250);		
					}	
			}
			
		enviroment_lightness     = analogRead(A0);

		
	}		
	
	Timer1.detachInterrupt();	
	poweroff();
}

void SETMODE_PAR()//SET Y
{	//cli(); 
	WTD.doggieTickle(); 
	digitalWrite(led ,LOW);
	Timer1.detachInterrupt();
	Timer1.initialize(10000);
	Timer1.attachInterrupt(Time1_flg);
	//detachInterrupt(0);
	pinMode(button, INPUT);
	int enviroment_lightness = 0;
	
	enviroment_lightness     = analogRead(A0);
	while(!flg)
	{
		WTD.doggieTickle();	
		
		if (NIGHT)
			{
				delay_feed(20);
				val=0;	
				while(NIGHT) WTD.doggieTickle();
				val_detecting=val;
				val=0;
				//Serial.println(val_detecting);
				if (val_detecting>5)	
					{
					par_num++;
					if (par_num==5)par_num=1;
					EEPROM.write(function_num,par_num);
					blink_led(par_num,250);		
					}				
			}
		
		
		if (DAY)
			{
				delay_feed(20);
				val=0;	
				while(DAY) WTD.doggieTickle();
				val_detecting=val;
				val=0;
				//Serial.println(val_detecting);
				if (val_detecting>5)	
					{
					par_num++;
					if (par_num==5)par_num=1;
					EEPROM.write(function_num,par_num);
					blink_led(par_num,250);		
					}	
			}
			
		enviroment_lightness     = analogRead(A0);

		
	}
	Timer1.detachInterrupt();	
	poweroff();
	//software_Reset();
	
	
	
}

void SETMODE_ALL()//SET X AND Y 
{	//cli(); 
	WTD.doggieTickle(); 
	digitalWrite(led ,LOW);
	Timer1.detachInterrupt();
	Timer1.initialize(5000);
	Timer1.attachInterrupt(Time1_flg);
	//detachInterrupt(0);
	pinMode(button, INPUT);
	
	
	int enviroment_lightness = 0;
	int change_lightness = 0;
	int sensorValue      = 0;
	unsigned int val_day = 0;
	unsigned int val_night = 0;
	long begin_night = 0;
	long duration_night   = 0;

	bool detect_flg = 0;
	
	while(!flg)
	{
		WTD.doggieTickle();
		//============guide code===========//
		enviroment_lightness     = analogRead(A0);//read
		delayMicroseconds(3500);
		sensorValue = analogRead(A0);
		
		val = 0;
		begin_night = millis();
		while(sensorValue-enviroment_lightness >80)
		{
			WTD.doggieTickle();
			sensorValue = analogRead(A0);
			//Serial.println(sensorValue-enviroment_lightness);
			duration_night = millis()-begin_night;
			if(duration_night>100)break;				
		}
		val_detecting = val;
		val = 0;		
		
		
		//Serial.println(val_detecting);
		
		if((val_detecting>18)&&(val_detecting<22))//judge : if the guide code is right.
		{
			val_day++;
			EEPROM.write(0,val_day);
			detect_flg = 1;
			//Serial.println("e");
		}
		
		
		
		while(detect_flg)//confirm guide code 
		{
			WTD.doggieTickle();
			//=============detect night from day============//
			enviroment_lightness     = analogRead(A0);//read
			delayMicroseconds(3500);
			sensorValue = analogRead(A0);
			
			val = 0;
			begin_night = millis();
			while(enviroment_lightness - sensorValue > 80)
			{
				WTD.doggieTickle();
				sensorValue = analogRead(A0);
				//Serial.println(enviroment_lightness - sensorValue);
				duration_night = millis()-begin_night;
				if(duration_night>1500)break;
			}
			val_detecting = val;
			val = 0;		
			//Serial.print("n");
			//Serial.println(val_detecting);
			
			if((val_detecting>47)&&(val_detecting<53))
				{
					val_day++;
					EEPROM.write(0,val_day);
					
				}
			if((val_detecting>290)&&(val_detecting<310))
				{	
					
					//=============detect day from night============//
					//val_day = 0;
					begin_night = millis();
					while(detect_flg)
					{
						WTD.doggieTickle();
						enviroment_lightness     = analogRead(A0);//read
						delayMicroseconds(3500);
						sensorValue = analogRead(A0);
						
						val = 0;
						while(sensorValue-enviroment_lightness >80)
						{
							WTD.doggieTickle();
							sensorValue = analogRead(A0);
							//Serial.println(sensorValue-enviroment_lightness);
						}
						val_detecting = val;
						val = 0;		
						//Serial.print("d");
						//Serial.println(val_detecting);
						
						if((val_detecting>47)&&(val_detecting<53))//judge : if the guide code is right.
						{
							val_night++;
							//Serial.println("enviroment_lightness");
						}
						duration_night = millis()-begin_night;
						if(duration_night>2100)break;
					}
					EEPROM.write(val_day,val_night);
					poweroff();
					//while(1);
				}
		}
	}
	Timer1.detachInterrupt();	
	poweroff();
}



void POWERON()
{
	WTD.doggieTickle();
	
	switch(EEPROM.read(0)) 
	{
		case 1://A-PWM
			B_B();
			break;
		case 2://A-SEGMENT
		 	B_SEGMENT();
			break;		
		case 3://IR-D
		 	IR_B();
			break;
		case 4://TIMING-D
		 	TIMING_B();
			break;	
	}	
}


void B_B()//X= 1;
{
	pinMode(3,OUTPUT);
	int val_analog = 0;
	
	switch(EEPROM.read(1))
	{
		case 1:
			while(!flg)
			{delay_feed(20);
			val_analog = analogRead(A5);
			val_analog = map(val_analog, 0, 1023, 0, 255);				
			analogWrite(3, val_analog);
			analogWrite(5, val_analog);
			detect_power();
			}				
			break;
			
		case 2://NOT
			while(!flg)
			{delay_feed(20);
			val_analog = analogRead(A5);
			val_analog = map(val_analog, 0, 1023, 0, 255);				
			analogWrite(3, 255-val_analog);	
			analogWrite(5, 255-val_analog);
			detect_power();
			}				
			break;
			
		case 3://AND
			while(!flg)
			{delay_feed(20);
			val_analog = analogRead(A5);
			val_analog = map(val_analog, 0, 1023, 0, 255);				
			if ((analogRead(A4)>800)&&(analogRead(A5)>800))
				{
					analogWrite(3, val_analog);
					analogWrite(5, val_analog);
				}
			detect_power();
			}			
			break;
			
		case 4://OR
			while(!flg)
			{delay_feed(20);
			val_analog = analogRead(A5);
			val_analog = map(val_analog, 0, 1023, 0, 255);				
			if ((analogRead(A4)>800)||(analogRead(A5)>800))
				{
					analogWrite(3, val_analog);
					analogWrite(5, val_analog);
				}
			detect_power();
			}				
			break;

	}
}

void B_SEGMENT()//X = 2;
{
	tm1637.init();
	tm1637.set(BRIGHT_TYPICAL);	
	tm1637.clearDisplay();
	tm1637.display(3,0);	
	
	int val_analog   = 0;
	int val_counting = 0;
	int analogRead_a5 = 0;
	
	uint8_t phase = 0;
	uint16_t analogvalue_rgbled = 0;
	float h = 0;
	float s = 0;
	float v = 0;
	int i_ = 0;
	float f = 0;
	float p = 0;
	float q = 0;
	float t = 0;
	float r = 0;
	float g = 0;
	float b = 0;
	uint8_t  rgb_color[3] ;
	//pixels.begin(); 
	switch(EEPROM.read(2))
	{
		case 1:// display value
			while(!flg)
			{
				WTD.doggieTickle(); 
				val_analog = analogRead(A5);
				display_num (val_analog);
				detect_power();
			}				
			break;
			
		case 2:// counting 
			while(!flg)
			{
				WTD.doggieTickle(); 
				if (analogRead(A5)>1000)
				{
					while(analogRead(A5)>1000) WTD.doggieTickle();
					val_counting++;
					if (val_counting==100)val_counting==0;
				}
				display_num (val_counting);
			detect_power();
			}				
			break;
			
		case 3:// Plant rgb
		
			//pixels.begin(); 


			while(!flg)
			{
				delay_feed(20);
				analogvalue_rgbled = analogRead(A5);  
				phase =  map(analogvalue_rgbled, 0, 700, 1, 80);	
				h = phase / 256.0;
				s = 1.0;
				v = 1.0;
				i_ = floor(h * 6);
				f = h * 6 - i_;
				p = v * (1 - s);
				q = v * (1 - f * s);
				t = v * (1 - (1 - f) * s);
				r = 0, g = 0, b = 0;
				switch(i_ % 6){
					case 0: r = v; g = t; b = p; break;
					case 1: r = q; g = v; b = p; break;
					case 2: r = p; g = v; b = t; break;
					case 3: r = p; g = q; b = v; break;
					case 4: r = t; g = p; b = v; break;
					case 5: r = v; g = p; b = q; break;
								}				
				rgb_color[0] = r * 255;
				rgb_color[1] = g * 255;	
				rgb_color[2] = b * 255;	
				
				rgb_color[0] = rgb_color[0] >>lightness_rgbled;
				rgb_color[1] = rgb_color[1] >>lightness_rgbled;	
				rgb_color[2] = rgb_color[2] >>lightness_rgbled;	
						
				for(int i=0;i<NUMPIXELS;i++)
				{
					pixels.setPixelColor(i, pixels.Color(rgb_color[0],rgb_color[1],rgb_color[2])); // Moderately bright green color.
					pixels.show(); 	
				}	
				detect_power();
			}				
			break;		
			
		case 4:// Sound RGB
			while(!flg)
			{
				delay_feed(20);
				if (analogRead(A5)>200)
				{
					action_rgbled_on (); 
					delay(50);
				} 
				else action_rgbled_off ();
				detect_power();
			}				
			break;							
	}
}




void IR_B()// X = 3;
{
	pinMode(3,OUTPUT);
	
    unsigned char airGamutValue = 0;
    unsigned int ultrasonicAirDistance = 0;	
	
	unsigned char red_ws2812   = 0;
	unsigned char green_ws2812 = 0;  
	unsigned char blue_ws2812  = 0; 	

	
		switch(EEPROM.read(3))
	{
		case 1://IR_B
			IR.Init(A5);
			while(!flg)//ir switch 
			{
				WTD.doggieTickle();//=========================start
			
				if(IR.IsDta())
				{
					int length= IR.Recv(dta);
					if ((dta[8]==162)&&(dta[9]==93))		//on
						{
						ir_switch = !ir_switch;
						if (ir_switch)digitalWrite(3,HIGH);
							else digitalWrite(3,LOW);
						}		
						
				}	
				
			detect_power();//============================end 
			}	
			break;

		case 2://IR piano
			IR.Init(A5);
			while(!flg)
			{
				WTD.doggieTickle();//=================================start
				if(IR.IsDta())
				{int length= IR.Recv(dta);
				//if ((dta[8]==104)&&(dta[9]==151))sound(0);
				if ((dta[8]==48)&&(dta[9]==207))sound(1);		
				if ((dta[8]==24)&&(dta[9]==231))sound(2);		
				if ((dta[8]==122)&&(dta[9]==133))sound(3);		
				if ((dta[8]==16)&&(dta[9]==239))sound(4);	
				if ((dta[8]==56)&&(dta[9]==199))sound(5);		
				if ((dta[8]==90)&&(dta[9]==165))sound(6);		
				if ((dta[8]==66)&&(dta[9]==189))sound(7);		
				if ((dta[8]==74)&&(dta[9]==181))sound(8);	
				//if ((dta[8]==82)&&(dta[9]==173))sound(9);					
				}	
			detect_power();//=====================================end
			}		
			break;
	
		case 3://AIR piano
			while(!flg)
			{
			WTD.doggieTickle();//======================================start 

			ultrasonicAirDistance = ultrasonicAir.MeasureInCentimeters(); 
			
			if     ((ultrasonicAirDistance>DISTANCE1)&&(ultrasonicAirDistance<=DISTANCE2))airGamutValue = 1;
			else if((ultrasonicAirDistance>DISTANCE2)&&(ultrasonicAirDistance<=DISTANCE3))airGamutValue = 2;
			else if((ultrasonicAirDistance>DISTANCE3)&&(ultrasonicAirDistance<=DISTANCE4))airGamutValue = 3;
			else if((ultrasonicAirDistance>DISTANCE4)&&(ultrasonicAirDistance<=DISTANCE5))airGamutValue = 4;
			else if((ultrasonicAirDistance>DISTANCE5)&&(ultrasonicAirDistance<=DISTANCE6))airGamutValue = 5;
			else if((ultrasonicAirDistance>DISTANCE6)&&(ultrasonicAirDistance<=DISTANCE7))airGamutValue = 6;
			else if((ultrasonicAirDistance>DISTANCE7)&&(ultrasonicAirDistance<=DISTANCE8))airGamutValue = 7;
			else if((ultrasonicAirDistance>DISTANCE8)&&(ultrasonicAirDistance<=DISTANCE9))airGamutValue = 8;
			else airGamutValue = 0;
			
			if(airGamutValue>0)
			{
				sound(airGamutValue);
			}
			delay(30);
		
			detect_power();//========================================end
			}				
			break;		
			
		case 4://color rgbled 
			Wire.begin(); 
			TCS3414CS_init();    
			pixels.begin();

			while(!flg)
			{
			WTD.doggieTickle();//========================================start

			readRGB();		  
	
			red_ws2812   = getRed_ws2812();
			green_ws2812 = getGreen_ws2812();  
			blue_ws2812  =  getBlue_ws2812(); 
			
			red_ws2812 = red_ws2812 >>3;//lightness
			green_ws2812 = green_ws2812 >>3;	
			blue_ws2812 = blue_ws2812 >>3;
			for(int i=0;i<NUMPIXELS;i++)
			 {
				pixels.setPixelColor(i, pixels.Color(red_ws2812,green_ws2812,blue_ws2812)); // Moderately bright green color.
				pixels.show(); 
			 }

		
			detect_power();//==========================================end
			}				
			break;		
	}

}


void TIMING_B()//X = 4;
{
	pinMode(3,OUTPUT);
	pinMode(5,OUTPUT);
	
	int8_t TimeDisp[] = {0x00,0x00,0x00,0x00};
	tm1637.init();
	tm1637.set(BRIGHT_TYPICAL);		
	
	float dustVal=0;
	float sum_dustVal=0; 
	int delayTime=280;
	int delayTime2=30;
	float offTime=9680;
	pinMode(A4,OUTPUT);
	digitalWrite(A4,LOW); 
	pinMode(A5, INPUT);
	
	
	myservo.attach(3); 
	previousMillis = millis();
	myservo.write(90); 
	
switch(EEPROM.read(4))
	{
		case 1: //Timing
			start_time_timing = millis();
			while(!flg)
			{
			WTD.doggieTickle();//============================================start
			
			duration_timing = millis() - start_time_timing;
			second = (duration_timing/1000)%60;
			minute = (duration_timing/1000)/60%60;
			TimeDisp[0] = minute / 10;
			TimeDisp[1] = minute % 10;
			TimeDisp[2] = second / 10;
			TimeDisp[3] = second % 10;			
			tm1637.point(1);

			tm1637.display(TimeDisp);
			
			
			
			
			detect_power();//============================================end
			}
			break;	
		
		
		
		
		case 2://count down
			pinMode(A5,OUTPUT);
			digitalWrite(A5,LOW);
			second = 0;
			start_time_timing = millis();
			while(!flg)
			{
			WTD.doggieTickle();//=========================================start
			
			duration_timing = millis() - start_time_timing;
			duration_timing = 180000 - duration_timing;
			if(duration_timing<0)
				{
					start_time_timing = millis();
					state_ = !state_;
					if (state_)digitalWrite(A5,HIGH);
					else digitalWrite(A5,LOW);					
				}
			
			second = (duration_timing/1000)%60;
			minute = (duration_timing/1000)/60%60;
			TimeDisp[0] = minute / 10;
			TimeDisp[1] = minute % 10;
			TimeDisp[2] = second / 10;
			TimeDisp[3] = second % 10;			
			tm1637.point(1);

			tm1637.display(TimeDisp);
			
			detect_power();//===========================================end
			}	
			break;	

		case 3://Groot
			while(!flg)
			{
				WTD.doggieTickle();//===========================================start
			
				currentMillis = millis();
					if(currentMillis - previousMillis > interval)
					{	
					
						int val_sound = (analogRead(A5));
						//Serial.println (val_sound);
						if ((val_sound > 150)&&(val_sound < 200))
						{		 		
							myservo.write(pos_begin - 10);   
							delay(5*10);
							myservo.write(pos_end + 10);  	
							delay(5*10);
						}	
						if ((val_sound > 200)&&(val_sound < 300))
						{		 
							myservo.write(pos_begin);   
							delay(5*20);
							myservo.write(pos_end);  	
							delay(5*20);
						}	
						if ((val_sound > 300)&&(val_sound < 350))
						{		 		
							myservo.write(pos_begin - 10);   
							delay(5*40);
							myservo.write(pos_end + 10);  	
							delay(5*40);
						}	
						if ((val_sound > 350)&&(val_sound < 400))
						{		 	
							myservo.write(pos_begin - 20);   
							delay(5*60);
							myservo.write(pos_end + 20);  	
							delay(5*60);
						}	
						if ((val_sound > 400)&&(val_sound < 450))
						{		 		
							myservo.write(pos_begin - 30);   
							delay(5*80);
							myservo.write(pos_end + 30);  	
							delay(5*80);
						}
						
						previousMillis = millis();
						
					delay(150);	
					}				
			

			
			
			detect_power();//============================================end
			}		
			break;
	
		case 4://dust sensor 
			while(!flg)
			{
			WTD.doggieTickle();//===========================================start
			
			for(int i=0;i<32;i++)
				{
					digitalWrite(A4,HIGH); 
					delayMicroseconds(delayTime);
					dustVal=analogRead(A5); 
					delayMicroseconds(delayTime2);
					digitalWrite(A4,LOW); 
					delayMicroseconds(offTime);	
					sum_dustVal = sum_dustVal+dustVal ;
					delay(40);
				} 

				dustVal = int(sum_dustVal)>>5;
				
				float val_dustsensor = (dustVal-20)*5/1024;
				int val_dustsensor_ = int(val_dustsensor*100);
				display_num (val_dustsensor_);
				sum_dustVal = 0;			
			
			detect_power();//===========================================end
			}		
			break;
	}

}









//*************************************************************************************************************************//
//*************************************************************************************************************************//

void sound(uint8_t note_index)
{
	for(int i=0;i<50;i++)   
 	{
		digitalWrite(3,HIGH);
		delayMicroseconds(BassTab[note_index]);
		digitalWrite(3,LOW);
		delayMicroseconds(BassTab[note_index]);
	}
}



void display_num (int val_analog)
{
	ListDisp[0] = val_analog%10;
	ListDisp[1] = (val_analog/10)%10;	
	ListDisp[2] = (val_analog/100)%10;
    ListDisp[3] = (val_analog/1000)%10;	
				
	if (ListDisp[3]!=0)
		{
			tm1637.display(0,ListDisp[3]);
			tm1637.display(1,ListDisp[2]); 
			tm1637.display(2,ListDisp[1]);
			tm1637.display(3,ListDisp[0]);			
		}
				
	if ((ListDisp[3]==0)&&(ListDisp[2]!=0))
		{tm1637.clearDisplay();
			tm1637.display(1,ListDisp[2]); 
			tm1637.display(2,ListDisp[1]);
			tm1637.display(3,ListDisp[0]);	
		}

	if ((ListDisp[3]==0)&&(ListDisp[2]==0)&&(ListDisp[1]!=0))
		{tm1637.clearDisplay();
			tm1637.display(2,ListDisp[1]);
			tm1637.display(3,ListDisp[0]);		
			}
	if ((ListDisp[3]==0)&&(ListDisp[2]==0)&&(ListDisp[1]==0)&&(ListDisp[0]!=0))
		{tm1637.clearDisplay();
			tm1637.display(3,ListDisp[0]);		
		}

}








void blink_led (int y,int period_time )
{	
		if (y==0)digitalWrite(led,LOW);
			else 
			{
				for(int i=0;i<y;i++)
					{
					//analogWrite(led,x);
					digitalWrite(led,HIGH);
					delay_feed(period_time);
					//analogWrite(led,0);
					digitalWrite(led,LOW);
					delay_feed(period_time);		
					}
			}
}


void blink_pwr (int y,int period_time )
{	
		if (y==0)digitalWrite(pwr,HIGH);
			else 
			{
				for(int i=0;i<y;i++)
					{
					//analogWrite(pwr,x);
					digitalWrite(pwr,LOW);
					delay_feed(period_time);
					//analogWrite(pwr,0);
					digitalWrite(pwr,HIGH);
					delay_feed(period_time);	
					}
			}
}

void poweroff()
{
	detachInterrupt(0);
	digitalWrite(pwr_hold,HIGH);
	pinMode(button,OUTPUT);
	digitalWrite(button,LOW);
	delay_feed(20);
	digitalWrite(button,HIGH);
}

void software_Reset() // 软启动系统，但不 复位寄存器和变量
{
        asm volatile ("  jmp 0");
}  


void Time1_flg()
{
val++;
//doggieTickle(); 
}

void start_timing()
{
	/*flg_val++;
	if (flg_val==3)poweroff();
		else flg =!flg;*/
	flg =!flg;
}

void delay_feed(int val_)
{
	delay(val_);
	WTD.doggieTickle(); 
	
}


void detect_power()
{
/*	
if(analogRead(A2)<700)
{
		for(int i=0;i<3;i++)
		{
			digitalWrite(batled,HIGH);
			delay(500);
			digitalWrite(batled,LOW);
			delay(500);
			doggieTickle();
		}
		poweroff();		
}		*/
	
	
	
}

void action_rgbled_on ()
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
	
	rgb_color[0] = rgb_color[0] >>lightness_rgbled;
	rgb_color[1] = rgb_color[1] >>lightness_rgbled;	
	rgb_color[2] = rgb_color[2] >>lightness_rgbled;		
	
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
		delay(30);
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





