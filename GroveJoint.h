#ifndef _GROVEJOINT_H_
#define _GROVEJOINT_H_

#define DEBUG          1
#define LOG(message)   Serial.print("[ ");       \
                       Serial.print(millis());   \
                       Serial.print(" ms] ");    \
                       Serial.println(message);                                    

#define LOG_VALUE(message, value)   Serial.print("[ ");       \
                                    Serial.print(millis());   \
                                    Serial.print(" ms] ");    \
                                    Serial.print(message);    \
                                    Serial.println(value);
#define  ON  1
#define  OFF 0
                                    
class Grove_Joint
{
  public:
    //Attributions
    int AmbientLight = 0;
    
    
    Grove_Joint();
    
    /* Function: void initHardware()
     * Description: 
     * Parameter: 
     * Return:
     */
    void initHardware();
    
    /* Function   : int initLightSensor(void)
     * Description: 
     * Parameter  : 
     * Return     : Ambient light value
     */
    int initLightSensor(void);
    
    /* Function   : int isLightSensorTriggered(void)
     * Description: 
     * Parameter  : 
     * Return     : 
     */
    bool isLightSensorTriggered(void);
    
    /* Function   : programConfig(int config)
     * Description: 
     * Parameter  : 
     * Return     : duration of button press down (Ms)
     */
    uint16_t checkButtonDown();    

    
    /* Function   : powerOff()
     * Description: software power off, 
     * Parameter  :  
     * Return     :
     */
    void powerOff();

    /* Function   : softwareReset()
     * Description: board reset by software, SP point to zero address
     * Parameter  : 
     * Return     :
     */
    void softwareReset();
    
    /* Function   : 
     * Description: return pins on joint board
     * Parameter  : 
     * Return     :
     */     
    int getPinNumberBUTTON(void);  
    int getPinNumberLIGHT_SENSOR(void);
    int getPinNumberCHRG_LED(void);
    int getPinNumberPWR_HOLD(void);
    int getPinNumberPWR(void);
    int getPinNumberKEY(void);
    int getPinNumberLED(void);
    int getPinNumberOUT_PIN1(void);
    int getPinNumberOUT_PIN2(void);
    int getPinNumberIN_PIN1(void);
    int getPinNumberIN_PIN2(void);
    int getPinNumberBATTERY_ADC(void);
    int getPinNumberBATTERY_LED(void);
    
    /* Function   : getBattery_value(void)
     * Description: 
     * Parameter  : 
     * Return     : Battery value
     */
    float getBatteryLevel(void);
    float batteryManager(void);
    void battery();
	
    void settingStrongLightTrigger(bool mode = false);
    bool isStrongLightTrigger();
    void batteryLedOn(bool on_off);
    void batteryInit();
	
  private:
    //hardware IO definition                              
    int LIGHT_SENSOR = A0;
    int CHRG_LED     = A3;  //low-level work
    int PWR_HOLD     = A1;  
    int PWR          = 6;   //low-level work
    int KEY          = 2;
    int LED          = 10;  
    int OUT_PIN1     = 3;   //normal output pin
    int OUT_PIN2     = 5;
    int IN_PIN1      = A5;  //normal input pin
    int IN_PIN2      = A4;
    int BATTERY_ADC  = A2;
    int BATTERY_LED  = A3;
    
    const float LowPowerValue      = 3.2;//3.2;    //3.2V
    const float BatteryAnalogRatio = 214.0;    //ratio = (analogue value) / (battery level);
    float BatteryLevel;
    
    bool STRONG_LIGHT_TRIGGER = false;
};
extern Grove_Joint Joint;
#endif

