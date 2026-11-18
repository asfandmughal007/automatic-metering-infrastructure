// Host tests for firmware/ami_meter/ami_meter.ino (Arduino Mega + XBee + LCD, final FYP firmware).
#include "mocks/Arduino.h"
#include "mocks/LiquidCrystal.h"
#include "mocks/XBee.h"
#include "mocks/EEPROM.h"
#include "mini_test.h"
void get_values();
void disp_val();
void lcd_display();
void receive_xbee();
void send_xbee();
float rms_volt();
float curr_cal();
float rms_curr();
float power_fact_ahmed();
unsigned long real_power();
int get_freq_ali();
void kwh_cal();
int* encode_dec(float);
double decode_dec(int, int);
void process_data(uint8_t[], uint8_t);
int volt_cal();
#include "../../firmware/ami_meter/ami_meter.ino"

int main() {
  std::printf("ami_meter.ino\n");
  setup();
  CHECK(Serial.out.find("kwh start:") != std::string::npos, "setup() initialises and prints starting kWh");

  sim_reset(); sim().mains_hz = 50;
  CHECK_NEAR(get_freq_ali(), 50, 1, "get_freq_ali() counts 50 Hz on the zero-cross input");

  sim_reset(); sim().v_offset = 512; sim().v_peak = 300;  // sketch computes max(raw)*0.33/1.414 (no offset removal)
  CHECK_NEAR(rms_volt(), (512 + 300) * 0.33 / 1.414, 3, "rms_volt() follows its own scaling (max*0.33/1.414)");

  sim_reset(); sim().i_peak = 100;  // 100 counts => 7.41 A peak => 5.24 A rms, minus 0.25 A trim
  CHECK_NEAR(rms_curr(), 100 * 75.75 / 1023 / 1.414 - 0.25, 0.2, "rms_curr() returns RMS amps (with its -0.25 A zero-error trim)");

  sim_reset(); sim().i_peak = 0;    // no load
  CHECK_NEAR(rms_curr(), 0, 0.01, "rms_curr() is clamped to 0 with no load");

  // kWh integration: 1000 W for one hour -> 1 kWh
  sim_reset(); name.real_power = 1000; now_kwh = 0; name.kwh = 0;
  delay(3600000UL); kwh_cal();
  CHECK_NEAR(name.kwh, 1.0, 0.01, "kwh_cal() integrates 1000 W over 1 h to 1 kWh");
  CHECK(EEPROM.read(addr) == 1, "kwh_cal() persists kWh to EEPROM (whole kWh only)");

  // Current lagging 30 degrees: correct power factor is cos(30) = 0.866
  sim_reset(); sim().phase_deg = 30; sim().i_peak = 100; name.frequency = 50; name.root_mean_square_current = 5;
  KNOWN_BUG(std::fabs(power_fact_ahmed() - 0.866) < 0.05, "power_fact_ahmed() returns cos(30deg)=0.866 (formula treats ms as seconds)");

  sim_reset(); name.root_mean_square_current = 0.2;
  CHECK_NEAR(power_fact_ahmed(), 1.0, 1e-9, "power_fact_ahmed() returns 1 below 0.5 A");

  // Full measurement + display + XBee send cycle
  sim_reset(); sim().v_peak = 300; sim().i_peak = 100; Serial.out.clear();
  get_values(); disp_val(); lcd_display(); send_xbee();
  CHECK(Serial.out.find("Frequency:") != std::string::npos && Serial.out.find("RMS Voltage") != std::string::npos, "disp_val() prints all meter fields");
  CHECK(xbee.sent.size() == 1 && xbee.sent[0].payload.size() == 12, "send_xbee() transmits a 12-byte frame");

  CHECK_NEAR(decode_dec(23000 % 256, 23000 / 256), 23000, 0.5, "decode_dec(low, high) rebuilds a value using base 256");
  SUMMARY();
}
