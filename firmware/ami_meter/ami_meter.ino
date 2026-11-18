// AMI smart-meter firmware (Arduino Mega 2560) - final FYP version.
//
// Reproduced from Appendix I of docs/report/Advanced-Metering-Infrastructure.pdf.
// Only edits versus the printed appendix (marked "REPO NOTE"):
//   1. A stray un-commented line ("store in EEPROM") that broke compilation is now a comment.
//   2. volt_cal() is called by power_fact_ahmed() but is not printed in the appendix;
//      it is restored here from the earlier prototype (firmware/prototype_meter).
//   3. The comment marker in front of "Broad Casting address" was lost in the appendix and is restored.
// Known defects of the original logic are listed in docs/KNOWN_ISSUES.md; they are intentionally
// NOT silently "fixed" so the code still matches what was submitted.
#include <LiquidCrystal.h>
#include <XBee.h>
#include <EEPROM.h>
int addr = 0; //address for the storing kwh
const uint8_t voltage_pin = A0;
const uint8_t current_pin = A1;
const uint8_t freq_pin = 50;
int number_of_samples = 500;
XBee xbee = XBee();
ZBRxResponse rx = ZBRxResponse(); 
XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000ffff); // Broad Casting address (REPO NOTE: "//" was missing in the printed appendix)
//for transmitting data
//ZBTxRequest tx = ZBTxRequest();
//VARIABLES FOR STORING THE BILLING VALUES
//LCD configuration
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
struct billing
{
//this is used for storing all the values
double power_factor = 0.0;
unsigned long real_power = 0.0;
double rms_current = 0.00;
float root_mean_square_voltage = 0.0;
double root_mean_square_current = 0.0;
int    frequency;
double kwh;
};
struct billing name;
double now_kwh;
void setup() {  
now_kwh = 0;
name.kwh =0; 
Serial.begin(115200);
//Serial2.begin(9600);
pinMode(voltage_pin, INPUT);
pinMode(current_pin, INPUT);
    pinMode(freq_pin, INPUT);
lcd.begin(16, 2);
// Print a message to the LCD.
lcd.print("Started");
//xbee configs
Serial3.begin(9600);
xbee.begin(Serial3);
  Serial.print("kwh start:");
  Serial.println(name.kwh);
}
void loop() {
get_values();
disp_val();
    lcd_display();
send_xbee();
receive_xbee();
}
// ---- Metering ----
float rms_volt() { 
  float maximum = 0;
  float ff = 0;
  for (int j = 0; j<1000;j++) //take 1000 samples
  {
    float x = analogRead(voltage_pin);
    //Serial.println(x);
    ff = x*(0.33);
    //if(ff>maximum)
    maximum = max(maximum, ff);
  }
 return maximum / 1.414;
}
float curr_cal() {
  float ff = 0;
  int calibration = 514;
  int x = analogRead(current_pin);
  if (x<calibration)
    ff = (calibration - x)*(0.0740);
  if (x>calibration)
    ff = (x - calibration)*(0.0740);
  return ff;
}
float rms_curr() {
  float maximum = 0;
  for (int j = 0; j<10000;j++)
  {
    float x = analogRead(current_pin);
    float ff = (514 - x)*(75.75 / 1023);
    if (ff>maximum)
      maximum = ff;
  }
  float rr= (maximum / 1.414) - 0.25;
  if (rr < 0) { rr = 0; }
  return rr; //0.21 offset//zero error
}
float power_fact_ahmed() {
  int x = volt_cal();
  int y = curr_cal();
  if (name.root_mean_square_current<0.5)
    return 1;
  while (x != 0) {
    x = volt_cal();
  }
  long a = millis();
  while (y != 0) {
    y = curr_cal();
  }
  long b = millis() - a;
  //Serial.println(b);
  return cos(2*3.14*name.frequency*b);
}
unsigned long real_power() {
  name.real_power = 0;
  int inst_voltage = 0;
  unsigned long inst_power = 0;
  unsigned long sum_inst_power = 0;
  unsigned long inst_current = 0;
  unsigned long real = 0;
  real = rms_volt()*rms_curr();
  return real;
}
int get_freq_ali()
{
  name.frequency = 0;
  //feed clock for frequncy 
  long now = millis();
  long up = now + 1000; //one second
  bool val;
  int currentState = 0;
  int previousState = 0;
  int counter = 0;
  while (up >= millis()) //this loop should run for one second 
  {
    val = digitalRead(freq_pin); // read input value
    if (val == HIGH) {
      currentState = 1;
    }
    else {
      currentState = 0;
    }
    if (currentState != previousState) {
      if (currentState == 1) {
        counter = counter + 1;
        //Serial.println(counter);
      }
    }
    previousState = currentState;
  }
  //time up 1sec
  return counter;
}
//
//KWH calculation
//
void kwh_cal()
 {
   double diff = millis() - now_kwh;
   double diff_hr = diff / 3600000;
   //name.kwh =EEPROM.read(addr);
   name.kwh =name.kwh + (name.real_power)*(diff_hr/1000);
   //name.kwh = name.kwh;
   now_kwh = millis();
   // REPO NOTE: was an un-commented stray line "store in EEPROM"
  EEPROM.write(addr,name.kwh);
}
//
void get_values() {
    //calculating meter values and storing them in the structure
    //define a structure name
    //get frequency
      name.root_mean_square_voltage = 0;
    name.frequency = get_freq_ali();
    //get rms current
    name.root_mean_square_current = rms_curr();
    //get rms voltage
    name.root_mean_square_voltage = rms_volt();
    //get the real power
    name.real_power = real_power();
    //get power factor
    name.power_factor = power_fact_ahmed();
    //kwh
      kwh_cal();
    delay(10);
}
void disp_val() {
  Serial.print("Frequency:     ");
  Serial.print(name.frequency);
  Serial.println("");
  Serial.print("power factor   ");
  Serial.print(name.power_factor);
  Serial.println("");
  Serial.print("Real_power     ");
  Serial.print(name.real_power);
  Serial.println("");
  Serial.print("RMS Voltage    ");
  Serial.print(name.root_mean_square_voltage);
  Serial.println("");
  Serial.print("RMS Current    ");
  Serial.print(name.root_mean_square_current);
    Serial.println("");
  Serial.print("KWH            ");
  Serial.print(name.kwh);
  Serial.println("");
}
void lcd_display() {
    lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Real Power: ");
  lcd.print(name.real_power);
    lcd.setCursor(0, 1);
  lcd.print("KWH : ");
  lcd.print(name.kwh);
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RMS V: ");
  lcd.print(name.root_mean_square_voltage);
  lcd.setCursor(0, 1);
  lcd.print("Frequency: ");
  lcd.print(name.frequency);
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PF : ");
  lcd.print(name.power_factor);
  lcd.setCursor(0, 1);
  lcd.print("RMS I: ");
  lcd.print(name.root_mean_square_current);
  /*lcd.print("RMS voltage");
  delay(300);*/
}
void receive_xbee()
{
  uint8_t option = 0;
  uint8_t data = 0;
  xbee.readPacket();
  if (xbee.getResponse().isAvailable()) {
    //Serial.println("data received");    
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) 
    {
      xbee.getResponse().getZBRxResponse(rx);
      process_data(rx.getData(), rx.getDataLength());
    }
  }
}
void send_xbee() {
  //XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000ffff);
  //ZBTxRequest tx = ZBTxRequest()
  //uint8_t payload[] = { name.frequency,name.power_factor,name.rms_current,name.root_mean_square_voltage };
  int * irms;
  int * pf;
  int *vol;
  int *RP;
  int *kwh;
  irms = encode_dec(name.root_mean_square_current);
  uint8_t rem_i = irms[0];
  uint8_t ans_i = irms[1];
  pf = encode_dec(name.power_factor);
  uint8_t rem_pf = pf[0];
  uint8_t ans_pf = pf[1];
  vol = encode_dec(name.root_mean_square_voltage);
  uint8_t rem_vol = vol[0];
  uint8_t ans_vol = vol[1];
  RP = encode_dec(name.real_power);
  uint8_t rem_RP = RP[0];
  uint8_t ans_RP = RP[1];
  kwh = encode_dec(name.kwh);
  uint8_t rem_kwh = kwh[0];
  uint8_t ans_kwh = kwh[1];
  uint8_t f=name.frequency;
  //decode_dec(rem_vol, ans_vol);
 uint8_t payload[] = {rem_i,ans_i,rem_pf,ans_pf, rem_vol, ans_vol, f,rem_RP,ans_RP,rem_kwh,ans_kwh,1}; //Sendng frame
  //uint8_t payload[] = {rem_i,ans_i,rem_pf,ans_pf, rem_vol, ans_vol, f,rem_RP,ans_RP,2};
  ZBTxRequest tx = ZBTxRequest(addr64, payload, sizeof(payload)); 
  xbee.send(tx);}
//  f,V,I,R,pf
int *encode_dec(float y) {
  int r[2];
  int x = y * 100;  
    r[0] = x % 256;
    r[1] = x / 255;
    //Serial.println(r[0]);
    Serial.flush();
    delay(20);
  return r;
}
double decode_dec(int rem, int ans) {
  int answer = (ans * 256) + rem;
  //Serial.println (answer); 
  //Serial.flush();
  return answer;
}
void process_data(uint8_t payload[],uint8_t size)
{
  float  Irms=decode_dec(payload[0], payload[1]);
  float  pf =decode_dec(payload[2], payload[3]);
  float  vol=decode_dec(payload[4], payload[5]);
  int    frequency=payload[6];
  float  rp = decode_dec(payload[7], payload[8]);
  float  kwh=decode_dec(payload[9], payload[10]);
  int    add=payload[11];
  //delay(3000);
}

// REPO NOTE: not printed in the report appendix; taken from firmware/prototype_meter.
int volt_cal() {
  int x = analogRead(voltage_pin);
  int volt_val = (x / 304) * 65;
  return volt_val;
}
