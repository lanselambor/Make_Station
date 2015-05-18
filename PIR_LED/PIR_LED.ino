

#define LIGHT_UP_TIME 4000//10*60*1000

int led = 3;  //led control output pin
int pir = A5;   //PIR_Sensor input pin

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(pir, INPUT);
  
}

void loop() {
  if(digitalRead(pir))
  {
    unsigned long begin = millis();
    
    for(int i=255; i >= 0; i--)
    {
      long y = cal_circle_y(i, 255);  
      Serial.println(y);    
      analogWrite(led,y);
      delay(10);      
    }    
    
    while(LIGHT_UP_TIME > millis() - begin)
    {
        if(digitalRead(pir))
        {
            begin = millis();
        }
    }
  }  
  for(int i = 0; i<=255; i++)
  {
    long y = cal_circle_y(i, 255);  
    Serial.println(y);    
    analogWrite(led,y);
    delay(10);      
  }    
  
  while(!digitalRead(pir));
}

//r * r = (x-r)*(x-r) + y * y 
long cal_circle_y(long x, long r)
{
    return sqrt((r * r) - (x - r) * (x - r));
}