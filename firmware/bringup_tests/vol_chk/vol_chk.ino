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
 // pinMode(2,INPUT);
 // pinMode(3,INPUT);
  //  EIFR = 1;
  //  EIFR = 2; //interrupts flags
 // attachInterrupt( 0,voltage_ISR,RISING );
 // attachInterrupt( 1,current_ISR,RISING );

  
}


void loop() {
Serial.println(analogRead(A2));
 // max_of_voltage();
 Serial.println(maximum);
 Serial.println(".......................");
// delay(1000);
}

/*int volt_cal(){
  int x=analogRead(voltage_pin);
  int volt_val=(x/304)*65;
  return volt_val;
  
  }
*/
float max_of_voltage(){
   float maximum=0;
   
   for (int j=0; j<20000;j++)
   { 
    float x=analogRead(voltage_pin);
    //Serial.println(x);
    float ff=x*(357/1023);
    //if(ff>maximum)
    //maximum=ff;
   maximum=max(maximum,ff);
  }

   
  // Serial.print("maxium= ");
  // Serial.println(a[0]);
  // Serial.println(maximum);
  // delay(20);
     
     return maximum;
    
  }
