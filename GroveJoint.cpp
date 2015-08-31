#include <Arduino.h>
#include "GroveJoint.h"


Grove_Joint::Grove_Joint()
{
  
}

/* Function: void initHardware()
 * Description: 
 * Parameter: 
 * Return:
 */
void Grove_Joint::initHardware()
{
  //pin initial
  pinMode(CHRG_LED, OUTPUT);     
  //pinMode(PWR, OUTPUT);  
  pinMode(LED , OUTPUT);
  //pinMode(KEY, INPUT);   //when using software poweroff uncomment this
  //pinMode(PWR_HOLD, OUTPUT);   
  //pinMode(LIGHT_SENSOR, INPUT);
  pinMode(BATTERY_ADC, INPUT);
  
  //digitalWrite(PWR, HIGH);      
  digitalWrite(CHRG_LED, HIGH); //power led  
  //digitalWrite(PWR_HOLD, LOW);  //hold the power so the button can be used,software power off needed    
  
  //AmbientLight = initLightSensor();

#if 1  
  //starting shining   
  analogWrite(LED, 5);
  delay(500);
  analogWrite(LED, 0);
  delay(500);
  analogWrite(LED, 5);
  delay(500);
  analogWrite(LED, 0);
#endif
  //get battery level
  batteryInit();  
}

/* Function   : int initLightSensor(void)
 * Description: 
 * Parameter  : 
 * Return     : Ambient light value
 */
int Grove_Joint::initLightSensor(void)
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
bool Grove_Joint::isLightSensorTriggered(void)
{
  long ambient_light = 0;
  for(int i=0;i<20;i++){
    ambient_light += analogRead(LIGHT_SENSOR);
    delayMicroseconds(10);
  } 
  ambient_light /= 20; 
  
  if(STRONG_LIGHT_TRIGGER) { //program configured by strong light sensor        
    if( (AmbientLight*2) < ambient_light ) {
        return true;
    }
  }    
  else {
    if( (AmbientLight/3) > ambient_light ) {
      return true;
    }
  }
  
  return false;
}

/* Function   : programConfig(int config)
 * Description: 
 * Parameter  :  
 * Return     : duration of button press down (Ms)
 */
uint16_t Grove_Joint::checkButtonDown()
{  
  uint16_t ret = 0;
  uint16_t blockCountermillis = 0;
  uint16_t time_duration = 0;
  
  if((HIGH==digitalRead(KEY)))
  {
    delay(5);
    if((HIGH==digitalRead(KEY)))
    {      
      while((HIGH==digitalRead(KEY)))
      {
        delayMicroseconds(998);
        blockCountermillis++;        
      }
      time_duration = blockCountermillis;
      ret = time_duration;
      
      #if DEBUG        
        LOG_VALUE("time_duration: ", time_duration);
      #endif      
    }
  }
  
  return ret;
}


/* Function   : powerOff()
 * Description: software power off, 
 * Parameter  :  
 * Return     :
 */
void Grove_Joint::powerOff()
{
  
  detachInterrupt(0); 
  digitalWrite(PWR_HOLD, HIGH);	 
  pinMode(KEY, OUTPUT);    
  digitalWrite(KEY, LOW);    
  //delay(20);
  digitalWrite(KEY, HIGH);
  pinMode(KEY, INPUT);      

}

/* Function   : softwareReset()
 * Description: board reset by software, SP point to zero address
 * Parameter  : 
 * Return     :
 */
void Grove_Joint::softwareReset() 
{
  asm volatile ("jmp 0");
}

/* Function   : 
 * Description: return pins on joint board
 * Parameter  : 
 * Return     :
 */

int Grove_Joint::getPinNumberBUTTON(void)
{
  return KEY;
}

int Grove_Joint::getPinNumberLIGHT_SENSOR(void)
{
  return LIGHT_SENSOR;
}

int Grove_Joint::getPinNumberCHRG_LED(void)
{
  return CHRG_LED;
}

int Grove_Joint::getPinNumberPWR_HOLD(void)
{
  return PWR_HOLD;
}

int Grove_Joint::getPinNumberPWR(void)
{
  return PWR;
}

int Grove_Joint::getPinNumberKEY(void)
{
  return KEY;
}

int Grove_Joint::getPinNumberLED(void)
{
  return LED;
}

int Grove_Joint::getPinNumberOUT_PIN1(void)
{
  return OUT_PIN1;
}

int Grove_Joint::getPinNumberOUT_PIN2(void)
{
  return OUT_PIN2;
}

int Grove_Joint::getPinNumberIN_PIN1(void)
{
  return IN_PIN1;
}

int Grove_Joint::getPinNumberIN_PIN2(void)
{
  return IN_PIN2;
}

int Grove_Joint::getPinNumberBATTERY_ADC(void)
{
  return BATTERY_ADC;
}

int Grove_Joint::getPinNumberBATTERY_LED(void)
{
  return BATTERY_LED;
}

float Grove_Joint::getBatteryLevel(void)
{
  long tmp = 0;
  for(int i=0;i<50;i++)
  {
    tmp += analogRead(BATTERY_ADC);
  }
  return (float)tmp/50.0/BatteryAnalogRatio;
}

/* Function   : BatteryManager(void)
 * Description: return pins on joint board
 * Parameter  : Detect battery level and protect it
 * Return     : Battery level with LPF, 
 */
float Grove_Joint::batteryManager(void)
{  
  static bool ledFlip = false;
  static unsigned long clock = millis();
  
  BatteryLevel = BatteryLevel * 0.6 + getBatteryLevel() * 0.4;  
  
  if(BatteryLevel <= LowPowerValue) 
  {    
	#if DEBUG
    LOG("Low battery"); 
    #endif 
	
    static unsigned long oneMinuteDown = millis();
	if(millis() - oneMinuteDown > 60000)  //one minute equals 60000ms
	{
		batteryLedOn(OFF);
		powerOff();
	}
	if(millis()-clock >= 100)
    {
      clock = millis();
      ledFlip ^= 1;   //Low battery led
    }
    batteryLedOn(ledFlip);       
  }
  else
  {
    batteryLedOn(ON);
  }

  return BatteryLevel;
}

void Grove_Joint::batteryInit()
{
	BatteryLevel = getBatteryLevel();
	batteryLedOn(ON);
}

/* Function   : battery
 * Description: return pins on joint board
 * Parameter  : 
 * Return     :
 */
void Grove_Joint::battery()
{
  static long clock = millis();
  
  if( (millis()-clock) > 500)
  {
    clock = millis();
    //Battery detecting and protecting 
    float battery = batteryManager();
    //float battery = joint.getBatteryLevel();
    
    #if DEBUG
    LOG_VALUE("battery is ", battery); 
    #endif   
  }
}

void Grove_Joint::settingStrongLightTrigger(bool mode)
{
  STRONG_LIGHT_TRIGGER = mode;  
}

bool Grove_Joint::isStrongLightTrigger()
{
  return STRONG_LIGHT_TRIGGER;
}

void Grove_Joint::batteryLedOn(bool on_off)
{
  digitalWrite(BATTERY_LED, !on_off);
}
Grove_Joint Joint;
/* --------------end of file ------------------ */
