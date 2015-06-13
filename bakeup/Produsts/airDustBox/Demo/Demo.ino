#include "TM1637.h"
#include "TimerOne.h"
#include "WatchDog.h"

//DeBug  switch 
#define  DEBUG   0

//hardware IO definition
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
#define BATTERY_ADC    A2
#define BATTERY_LED    A3

#define LOWPOWER         725    //3.4V
#define SHUTDOWN_POWER   705    //3.3v

int clk_pin  = 3;
int data_pin = 5;
int dustPin  = A5;
int ledPower = A4;
int ledFlip  = 1;
float battery  = 0.0;

int   delayTime   = 280;
int   delayTime2  = 30;
int   offTime     = 9680;
float dustVal     = 0;
float sum_dustVal = 0;
  
TM1637 DustPPM_display(clk_pin, data_pin);

void setup()
{
  //watch dog
  WTD.watchdogSetup();
  WTD.doggieTickle();
  
  //power up
  pinMode(PWR, OUTPUT);
  digitalWrite(PWR, LOW);
  
  pinMode(BATTERY_ADC, INPUT);
  pinMode(BATTERY_LED, OUTPUT);
  digitalWrite(BATTERY_LED, HIGH);
  battery = aveFilter(BATTERY_ADC, 100);//analogRead(BATTERY_ADC);
  
  //dislpay init
  DustPPM_display.init();
  DustPPM_display.set(BRIGHT_TYPICAL);    
  DustPPM_display.point(false);
  DustPPM_display.clearDisplay();
  
  //start shine
  pinMode (LED,OUTPUT);
  for(int i=0;i<2;i++)
  {
    analogWrite(LED,5);
    delay(500);
    analogWrite(LED,0);     
    delay(500);          
  }
  
  Timer1.initialize(500000);  //500ms
  Timer1.attachInterrupt(isr);
  #if DEBUG
  Serial.begin(9600);
  Serial.println("start...");
  #endif
}

void loop()
{
  static int display_Flip = 1;
  display_Flip = !display_Flip;
  
  for(int i=0;i<10;i++)
	{
		digitalWrite(ledPower,HIGH); 
		delayMicroseconds(delayTime);
		dustVal=analogRead(dustPin); 
		delayMicroseconds(delayTime2);
		digitalWrite(ledPower,LOW); 
		delayMicroseconds(offTime);	
		sum_dustVal = sum_dustVal+dustVal;
    delay(40);
    WTD.doggieTickle();		
	} 
  
	dustVal = sum_dustVal/10;
	sum_dustVal = 0;
  
	dustVal = dustVal*5/1024;
  dustVal = dustVal*1000/6-100;
  
  #if DEBUG
  Serial.print("dust ");
  Serial.println(dustVal);
  #endif
  
  displayNumber(dustVal);   
  #if 0
  if(display_Flip) displayNumber(dustVal);   
  else displayNumber(battery);
  #endif
  WTD.doggieTickle();
  
}
void isr()
{
  baterry_ckeck();    
}
  
void displayNumber(int number)
{
  int thousand = 0;
  int hundred = 0;
  int tens = 0;
  int units = 0;
  
  thousand = number / 1000;
  hundred = number % 1000 / 100;
  tens = number % 100 / 10;
  units = number % 10;   
  
  if(thousand != 0)  DustPPM_display.display(0,thousand);
  if((thousand != 0) || (hundred != 0))  DustPPM_display.display(1,hundred);
  if((thousand != 0) || (hundred != 0) || (tens != 0))  DustPPM_display.display(2,tens);
  DustPPM_display.display(3,units);
  
}

int baterry_ckeck(void)
{  
  battery = battery*0.9 + aveFilter(BATTERY_ADC, 100)*0.1;
  #if DEBUG
  Serial.print("Battery: ");
  Serial.println(battery);
  #endif
    
  ledFlip = !ledFlip;  
  if(battery < SHUTDOWN_POWER)
  {
    powerOff();
  }
  else if(battery < LOWPOWER)
  {            
    if(ledFlip)   digitalWrite(BATTERY_LED, LOW);
    else  digitalWrite(BATTERY_LED, HIGH);
  }
#if 0
  if(ledFlip){
    displayNumber(battery);  
  }
  else displayNumber(dustVal);   
#endif  
}

int aveFilter(int analogPin, int num)
{
  long tmp = 0;
  for(int i=0; i<num; i++)
  {
    tmp += analogRead(analogPin);
  }
  tmp /= num;
  return tmp;
}

/* Function   : powerOff()
 * Description: software power off, 
 * Parameter  :  
 * Return     :
 */
void powerOff()
{ 
  digitalWrite(PWR_HOLD, HIGH);	      
  pinMode(BUTTON, OUTPUT);    
  digitalWrite(BUTTON, LOW);    
  delay_feed(20);
  digitalWrite(BUTTON, HIGH);  
  pinMode(BUTTON, INPUT);    

}
void delay_feed(int ms)
{
  for(int i=0;i<ms;i++)
  {
    delayMicroseconds(998);
    WTD.doggieTickle();
  }
}









