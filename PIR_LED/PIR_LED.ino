
#define LIGHT_UP_TIME 10000//10*60*1000

int motor = 3;  //motor control output pin
int pir = A5;   //PIR_Sensor input pin

void setup() {
  pinMode(motor, OUTPUT);
  pinMode(pir, INPUT);
  
}

void loop() {  
  if(digitalRead(pir))
  {
    unsigned long begin = millis();
    
    digitalWrite(motor,LOW); 
    
    while(LIGHT_UP_TIME > millis() - begin)
    {
        if(digitalRead(pir))
        {
            begin = millis();
        }
    }           
  }  
  digitalWrite(motor,HIGH);     
}