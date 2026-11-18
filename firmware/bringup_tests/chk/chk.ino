int count=0;
int voltage_value=0;
int current_value=0;
const int voltage_pin=A2; //frequency calculation input
const int current_pin=A1;
int freq;
volatile int x=0,y=0;


void setup() {
  Serial.begin(9600);
    // put your setup code here, to run once:
  
  pinMode(voltage_pin,INPUT);
  pinMode(current_pin,INPUT);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  //  EIFR = 1;
  //  EIFR = 2; //interrupts flags
 // attachInterrupt( 0,voltage_ISR,RISING );
 // attachInterrupt( 1,current_ISR,RISING );

  
}


void loop() {
 float initial=millis();
  Serial.print("initial= ");
 Serial.println(initial);
 for (int j=0; j<10000;j++)
   { 
    float x=analogRead(voltage_pin);
    float y=analogRead(voltage_pin);
    float z=analogRead(voltage_pin);
    float a=analogRead(voltage_pin);
    
    //Serial.println(x);
   
  }
 float final=millis();
   Serial.print("final= ");
 Serial.println(final);
}
