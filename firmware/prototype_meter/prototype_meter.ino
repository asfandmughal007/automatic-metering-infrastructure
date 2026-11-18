int count=0;
int voltage_value=0;
int current_value=0;
const int voltage_pin=A2; //frequency calculation input
const int current_pin=A1;
int freq;
volatile int x=0,y=0;

void voltage_ISR()
{
    EIFR = 1;
    x=2;
    //Serial.println("v");
    
    
    
}
void current_ISR()
{    
  EIFR = 2;
    // Toggle LED
    //Serial.println("c");
    if(x==2)
    y=2;
    
    
}
void setup() {
  Serial.begin(9600);
    // put your setup code here, to run once:
  
  pinMode(voltage_pin,INPUT);
  pinMode(current_pin,INPUT);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  //  EIFR = 1;
  //  EIFR = 2; //interrupts flags
  attachInterrupt( 0,voltage_ISR,RISING );
  attachInterrupt( 1,current_ISR,RISING );

  
}


void loop() {
 float current_rms;
 float voltage_rms;

  float power=0;
  /* while(x==0);  
  int initial=millis();
  detachInterrupt(0);  
  while(y==0);  
  detachInterrupt(1);
  int final=millis();
  int delta=final-initial;  
  y=0;  x=0;
 */
 int frequency=freq_cal();

 
// float phase=(delta/1000)*360*frequency;//phase calculation formula 
  float phase=0.9; //
 int y2=millis()+10000;
 while(millis()<y2){
  voltage_rms=max_of_voltage()/1.4142;

  current_rms=max_of_current()/1.4142;

 

 //power=vrms*Irms*pf;


 float power=power+voltage_rms*current_rms*phase;

 }
 
 float kwh=power*360;//power per unit time(10sec)

 Serial.print("powerfactor= ");
 Serial.println(phase);

 
 Serial.print("frequency= ");
 Serial.println(frequency);

 
 Serial.print("I_rms= ");
 Serial.println(current_rms);

 

 Serial.print("V_rms= ");
 Serial.println(voltage_rms);

  Serial.print("kwh= ");
 Serial.println(kwh);


  Serial.println("****_____________********");
 delay(1000);
 
  attachInterrupt( 0,voltage_ISR,RISING );
  attachInterrupt( 1,current_ISR,RISING );

  
}



void notify(int x){
    for(int i=0;i<x;i++){
 digitalWrite(13,HIGH);
  delay(200);
  digitalWrite(13,LOW);
  delay(200);
  }
 }
//freq calcutaion ___________________________________________

int freq_cal(){
int initial_time=millis();
int final_time=initial_time+1000;

  while(initial_time<final_time){        //1 second loop
    
 voltage_value=analogRead(voltage_pin);
                if(voltage_value>512){
  
               while(voltage_value>512){
                voltage_value=analogRead(voltage_pin);                
                }
              count++;
                }

                if(voltage_value<512){                  
                  while(voltage_value<512){
                    voltage_value=analogRead(voltage_pin);                    
                    }
                  count++;
                  
                  }
  
initial_time=millis();
}
//frequency loop ended
//Serial.println("1 second ended");
int z=count/2;
count=0;
return z;  
  }
 //*****************___________________________***************************

float max_of_voltage(){
   float maximum=0;
   
   for (int j=0; j<10000;j++)
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

  float min_of_voltage(){ 
   float minimum=0;   
   for (int j=0; j<10000;j++){
     
   int x=analogRead(voltage_pin); 
   float ff=x*(357/1023); 
   //if(ff<minimum)
   //minimum=ff;
   minimum=min(minimum,ff);
   }    
   //Serial.print("minimum= ");   
   return minimum;
    
  }

int volt_cal(){
  int x=analogRead(voltage_pin);
  int volt_val=(x/304)*65;
  return volt_val;
  
  }
float current_cal(){
 // int x=max_of_pin(current_pin);
 // float c_val=(505-x)*(75.75/1023);
    
  
  
//  return c_val;
  }

float max_of_current(){
   float maximum=0;
   
   for (int j=0; j<10000;j++)
   { 
    float x=analogRead(current_pin);
    float ff=(516-x)*(75.75/1023);
    if(ff>maximum)
    maximum=ff;
  }

   
  // Serial.print("maxium= ");
  // Serial.println(a[0]);
  
     return maximum;
    
  }

float min_of_current(){ 
   float minimum=0;   
   for (int j=0; j<10000;j++){     
   int x=analogRead(current_pin); 
   float ff=(516-x)*(75.75/1023);  
   if(ff<minimum)
   minimum=ff;}    
   //Serial.print("minimum= ");   
   return minimum;
    
  }



 
