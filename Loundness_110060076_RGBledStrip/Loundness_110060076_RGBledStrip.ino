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
  if(millis() - resetTime > TIMEOUTPERIOD){
    doggieTickle();                                          
	resetFunc();     
  }
}

//DeBug  switch 
#define  DeBug   0


//Libraries
#include <Adafruit_NeoPixel.h>
#define PIN            3
#define NUMPIXELS      20
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define lightness  3

int val_sound = 0;         

// =========  Setup  =========
void setup()
{
	
    pixels.begin(); 
	
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
    delay(50);
    val_sound = analogRead(A5);   
	
#if DeBug  	
    Serial.println(val_sound);  
#endif  	

    if (val_sound>200)action_rgbled_on ();  
	    else action_rgbled_off ();
	
	

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
		delay(20);
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

