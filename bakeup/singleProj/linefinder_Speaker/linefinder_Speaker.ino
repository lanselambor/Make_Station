//DeBug  switch 
#define  DeBug   0

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

int linefinder = IN_PIN1;
int recorder   = OUT_PIN1;

const uint8_t ON = 1;
const uint8_t OFF = 0;
const uint32_t speakDuration = 1 * 1000;

void setup() {
  pinMode(linefinder, INPUT);
  //digitalWrite(linefinder, LOW);
  pinMode(recorder, OUTPUT);
}

void loop() {     
  if(digitalRead(linefinder) == HIGH)
  {         
    for(int i=0; i<3; i++)
    {
      uint32_t timeCount = millis();
      digitalWrite(recorder, HIGH);
      while(millis() - timeCount < speakDuration)
      {        
        if(digitalRead(linefinder) == LOW)
        {
          i=3;          
          digitalWrite(recorder, LOW);            
          break;
        }
      }
      digitalWrite(recorder, LOW);
      delay(500);
    }    
  }
  while(digitalRead(linefinder) == HIGH);
  while(digitalRead(linefinder) == LOW);
}
