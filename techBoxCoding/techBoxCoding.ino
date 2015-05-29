#include <TimerOne.h>
#include "TM1637.h"
#include <EEPROM.h>
#include <Wire.h>

#define DEBUG          1
#define LOG(message)   Serial.print("[ ");       \
                              Serial.print(millis());   \
                              Serial.print(" ms] ");    \
                              Serial.print(message);                                    

#define LOG_VALUE(message, value)   Serial.print("[ ");       \
                                    Serial.print(millis());   \
                                    Serial.print(" ms] ");    \
                                    Serial.print(message);    \
                                    Serial.print(value);  
                              

#define BUTTON         2
#define LIGHT_SENSOR   A0
#define CHRG_LED       A3
#define PWR_HOLD       A1
#define PWR            6
#define KEY            2
#define LED            10
#define OUT_PIN1       3
#define OUT_PIN2       5
#define IN_PIN1        A4
#define IN_PIN2        A5

#define NORMAL_BUTTON_EVENT   1
#define POWER_OFF             2
#define PROGRAM_CONFIG        3
#define LIGHT_SENSOR_TRIGGER    ( HIGH==digitalRead(LIGHT_SENSOR) )
#define STRONG_LIGHT_TRIGGER    false
#define DARK_LIGHT_TRIGGER      true

#define TRIGGERED_BY_BUTTON           true
#define TRIGGERED_BY_LIGHTSENSOR      false

#define BUTTON_DOWN             ( HIGH==digitalRead(BUTTON) )
#define BUTTON_SHORT_PRESS      300  //300ms
#define BUTTON_NORMAL_PRESS     1000 //1000ms
#define BUTTON_LONG_PRESS       3000 //3000ms

int AmbientLight = 0;
int CurrentProgramNumber = 0;

void setup() 
{      
  #if DEBUG 
  Serial.begin(9600);
  delay(1000);
  LOG("begin....");      
  #endif 

  initHardware();  
}

void loop() 
{
  
  /* ------------- 1.Program confiuraton ----------- */

  if(NULL != CurrentProgramNumber)
  {
    #if DEBUG
    LOG_VALUE("CurrentProgramNumber is ", CurrentProgramNumber);    
    #endif
    
  }
  /* -------------- 2.Main Routine ---------------- */
  //mainRoutine(CurrentProgramNumber);
  /* ------------- 3. ---------------------------- */
  
  
}

/* Function: void initHardware()
 * Description: 
 * Parameter: 
 * Return:
 */
void initHardware()
{
  //pin initial
  pinMode(CHRG_LED, OUTPUT);   
  pinMode(PWR, OUTPUT);            
  pinMode(LED , OUTPUT);
  pinMode(KEY, INPUT);
  pinMode(PWR_HOLD, OUTPUT);   
  pinMode(LIGHT_SENSOR, INPUT);
  
  digitalWrite(PWR, LOW);  //light up the red led
  digitalWrite(PWR_HOLD, LOW);  //hold the power so the button can be used    
  
  AmbientLight = initLightSensor();

  
  //starting shining   
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(500);
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(500);  

  attachInterrupt(0, keyRoutine, RISING);
}

/* Function   : void keyRoutine()
 * Description: deal with key action
 * Parameter  : 
 * Return     : 
 */
void keyRoutine()
{
  detachInterrupt(0);
  int config = checkButtonDown();
  CurrentProgramNumber = programConfig(config);
  attachInterrupt(0, keyRoutine, RISING);
  
}

/* Function   : int initLightSensor(void)
 * Description: 
 * Parameter  : 
 * Return     : Ambient light value
 */
int initLightSensor(void)
{
  long tmp = 0;
  for(int i=0;i<100;i++){
    tmp += analogRead(LIGHT_SENSOR);
  } 
  tmp /= 100;
  #if DEBUG
  LOG_VALUE(" Read Light Sensor: ", tmp);  
  #endif
  
  return tmp;
} 

/* Function   : int isLightSensorTriggered(void)
 * Description: 
 * Parameter  : 
 * Return     : 
 */
bool isLightSensorTriggered(void)
{
  long ambient_light = 0;
  for(int i=0;i<50;i++){
    ambient_light += analogRead(LIGHT_SENSOR);
  } 
  ambient_light /= 50; 
  
#if STRONG_LIGHT_TRIGGER  //program configured by strong light sensor        
  if( (AmbientLight*2) < ambient_light )
  {

#elif DARK_LIGHT_TRIGGER  
  if( (AmbientLight/2) > ambient_light )
  {
#endif     
    return true;
  }
  
  return false;
}
/* Function   : programConfig(int config)
 * Description: 
 * Parameter  : 
 * Return     :
 */
int checkButtonDown()
{
  int ret = 0;    
  if( BUTTON_DOWN )
  {
    delay(5);
    if( BUTTON_DOWN )
    {
      int time_begin = millis();
      int time_duration = 0;
      while( BUTTON_DOWN )
      {
        delay(20);       
      }
      
      time_duration = millis() - time_begin;
      
      if(BUTTON_SHORT_PRESS > time_duration)
      {
        //power switch
        ret = BUTTON_SHORT_PRESS;
        
        #if DEBUG        
        LOG_VALUE("button press without ", BUTTON_SHORT_PRESS);
        #endif        
      }
      else if(BUTTON_NORMAL_PRESS > time_duration)
      {
        //program configuration
        ret = BUTTON_NORMAL_PRESS;
        #if DEBUG        
        LOG_VALUE("button press without ", BUTTON_NORMAL_PRESS);
        #endif        
      }
      else if(BUTTON_LONG_PRESS < time_duration)
      {
        //program configuration
        ret = BUTTON_LONG_PRESS;
        #if DEBUG        
        LOG_VALUE("button press outof ", BUTTON_LONG_PRESS);
        #endif        
      }
    }
  }
  
  return ret;
}

/* Function   : programConfig(int config)
 * Description: 
 * Parameter  : 
 * Return     :
 */
int programConfig(int config)
{
  if(config == POWER_OFF)
  {
    //powerOff();
  }
  else if(config == PROGRAM_CONFIG)
  {
    #if DEBUG
    LOG("4 digit display...");
    #endif
    
    TM1637 tm1637(OUT_PIN1, OUT_PIN2);
    tm1637.init();
    tm1637.set(BRIGHT_TYPICAL);    
    tm1637.point(false);
    tm1637.clearDisplay();
    
    tm1637.display(2,0);    
    tm1637.display(3,0);

    int programNumber = 0;
    int tens = 0;
    int units = 0;
    
    while( !BUTTON_DOWN )
    {
#if TRIGGERED_BY_BUTTON      
      if( checkButtonDown() )
      {//通过按键时间判断按键功能

#elif TRIGGERED_BY_LIGHTSENSOR
      if(isLightSensorTriggered())
      {
#endif
        programNumber++;
        if(16 < programNumber) programNumber = 1;
        tens = programNumber / 10;
        units = programNumber % 10;                       
        tm1637.display(2,tens);
        tm1637.display(3,units);
      }
      LOG("Configuring program number");
      delay(500);
    }
    
    return programNumber;    
  }
  
  return 0;
  
}
/* Function   : powerOff()
 * Description: software power off, 
 * Parameter  :  
 * Return     :
 */
void powerOff()
{
  detachInterrupt(0);  
  pinMode(PWR_HOLD, OUTPUT);  
  digitalWrite(PWR_HOLD, HIGH);	    
	pinMode(BUTTON, OUTPUT);    
  digitalWrite(BUTTON, LOW);    
	delay(20);
	digitalWrite(BUTTON, HIGH);
}

/* Function   : softwareReset()
 * Description: board reset by software, SP point to zero address
 * Parameter  : 
 * Return     :
 */
void softwareReset() 
{
  asm volatile ("jmp 0");
}











