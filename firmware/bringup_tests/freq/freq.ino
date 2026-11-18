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
freq_cal();
}

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
