int count=0;
int voltage_value=0;
int current_value=0;
const int voltage_pin=A2; //frequency calculation input
const int current_pin=A1;
int freq;
volatile int x=0,y=0;
float maximum;

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
max_of_current();
 Serial.println(current_pin);
}

float max_of_current(){
   float maximum=0;
   
   for (int j=0; j<10000;j++)
   { 
    float x=analogRead(current_pin);
    float ff=(516-x)*(75.75/1023);
    maximum=max(maximum,ff);
  }
    return maximum;
    
  }
