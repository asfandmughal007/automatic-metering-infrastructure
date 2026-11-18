int count=0;
int voltage_value=0;
int current_value=0;
const int voltage_pin=A2; //frequency calculation input
const int current_pin=A1;
int freq;
volatile int x=0,y=0;

void setup()
{
  pinMode(voltage_pin,INPUT);
  pinMode(current_pin,INPUT);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
}
void loop()
{
phase_cal();
}

void phase_cal(){
  Serial.println("phase_cal() function starts");
  float time_delay=0;
int  voltage_value=analogRead(voltage_pin);
int  current_value=analogRead(current_pin);
 float initial=0,final=0;
 while(voltage_value!=0){voltage_value=analogRead(voltage_pin);}
  
  initial=millis();
  Serial.print("initial timer= ");
  Serial.println(initial);
  while(current_value!=0){current_value=analogRead(current_pin);}
  Serial.print("final timer= ");
  
  final=millis();
  Serial.println(final);
 time_delay=final-initial;
 Serial.print("time delay= ");
 Serial.println(time_delay);
 

 float f=60;
//Serial.println(f);
 float ph=(f*360*time_delay)/1000;
 
 //return time_delay;
 Serial.print("phase= ");
 Serial.println(ph);
 Serial.println("________________");
}
