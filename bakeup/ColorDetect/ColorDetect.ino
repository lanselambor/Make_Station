#include <GroveColorSensor.h>
#include "Adafruit_NeoPixel.h"

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

#define NUMPIXELS      20
#define LIGHT_PIN      3
#define sound_pin      A5
#define PIN            LIGHT_PIN

class MyColorSensor : public GroveColorSensor
{
  public:
    int r, g, b, c;
    void readRGB()
    {
      uint8_t dataRead[8];
      Wire.beginTransmission(COLOR_SENSOR_ADDR);
      Wire.write(REG_BLOCK_READ);
      Wire.endTransmission();
  	
      Wire.beginTransmission(COLOR_SENSOR_ADDR);
      Wire.requestFrom(COLOR_SENSOR_ADDR, 8);
      delay(100);
      
      if(8 <= Wire.available())
      {
        int i;
        for(i = 0; i < 8; ++i)
        {
          dataRead[i] = Wire.read();	
        }
      }
      g = (int)(dataRead[1]*256 + dataRead[0]);
      r = (int)(dataRead[3]*256 + dataRead[2]);
      b = (int)(dataRead[5]*256 + dataRead[4]);
      c = (int)(dataRead[7]*256 + dataRead[6]);
    }
};

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  pixels.begin();  
  Serial.begin(9600);
}

void loop()
{
	MyColorSensor colorSensor;
	while(1)
	{
		colorSensor.readRGB();
    for(int i = 0;i<NUMPIXELS; i++)
    {
      pixels.setPixelColor(i, pixels.Color(colorSensor.r, colorSensor.g, colorSensor.b));
    }
    pixels.show();
		colorSensor.clearInterrupt(); 
	}
}
