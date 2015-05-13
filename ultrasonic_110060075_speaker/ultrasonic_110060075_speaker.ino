//watchdog setting
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






#include <Ultrasonic.h>
#define PWM_OUT_PIN       3
#define ULTRASONIC_PIN    A5
#define DEFAULT_DISTANCE  1
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
unsigned int noteDurations = 50 ;



#define  DeBug   0





// =========  setup  =========
void setup()
{ 

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


// =========  loop  =========
void loop()
{
    unsigned char airGamutValue = 0;
    unsigned int ultrasonicAirDistance = 0;
    doggieTickle(); 
	
	
    while(1)
    {                
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
#if DeBug             
			Serial.print("Air Gamut Value is:"); 
            Serial.println(airGamutValue); 
#endif             
            Gamut_Play(airGamutValue);
        }
        delay(30);
        //feed();
		doggieTickle();   
    }
}

void Gamut_Play(unsigned char data)
{
    switch(data)
    {      
        case 1:
        tone(PWM_OUT_PIN,523,noteDurations);//Do(523Hz)NOTE_C5
        break;        
        case 2:
        tone(PWM_OUT_PIN,587,noteDurations);//Re(587Hz)NOTE_D5
        break;        
        case 3:
        tone(PWM_OUT_PIN,659,noteDurations);//Mi(659Hz)NOTE_E5
        break;        
        case 4:
        tone(PWM_OUT_PIN,698,noteDurations);//Fa(698Hz)NOTE_F5
        break;        
        case 5:
        tone(PWM_OUT_PIN,784,noteDurations);//So(784Hz)NOTE_G5
        break;        
        case 6:
        tone(PWM_OUT_PIN,880,noteDurations);//La(880Hz)NOTE_A5
        break;       
        case 7:
        tone(PWM_OUT_PIN,988,noteDurations);//Si(988Hz)NOTE_B5
        break;       
        case 8:
        tone(PWM_OUT_PIN,1047,noteDurations);//Do(1047Hz)NOTE_C6
        break;             
        default:
        break;         
    }
}



