// Host tests for firmware/prototype_meter/prototype_meter.ino (Arduino Uno, first measurement prototype).
#include "mocks/Arduino.h"
#include "mini_test.h"
// The Arduino IDE generates these prototypes automatically; plain C++ needs them written out.
int freq_cal();
float max_of_voltage();
float max_of_current();
float min_of_voltage();
float min_of_current();
int volt_cal();
float current_cal();
void notify(int);
#include "../../firmware/prototype_meter/prototype_meter.ino"

int main() {
  std::printf("prototype_meter.ino\n");

  sim_reset(); sim().mains_hz = 50;
  CHECK_NEAR(freq_cal(), 50, 1, "freq_cal() measures 50 Hz mains");

  sim_reset(); sim().mains_hz = 60;
  CHECK_NEAR(freq_cal(), 60, 1, "freq_cal() measures 60 Hz mains");

  sim_reset(); sim().i_peak = 100;  // 100 counts * 75.75/1023 = 7.41 A peak
  CHECK_NEAR(max_of_current(), 100 * 75.75 / 1023, 0.15, "max_of_current() returns peak current in A");

  sim_reset(); sim().v_peak = 300;  // expected 300 counts * 357/1023 = 104.7 V
  KNOWN_BUG(max_of_voltage() > 50, "max_of_voltage() returns a non-zero volt value (357/1023 is integer division = 0)");

  sim_reset(); sim().v_peak = 300; sim().i_peak = 100; Serial.out.clear();
  loop();
  const std::string& o = Serial.out;
  CHECK(o.find("frequency= 50") != std::string::npos, "loop() prints frequency= 50");
  CHECK(o.find("I_rms= ") != std::string::npos && o.find("V_rms= ") != std::string::npos, "loop() prints I_rms and V_rms");
  CHECK_NEAR(std::atof(o.c_str() + o.find("I_rms= ") + 7), 100 * 75.75 / 1023 / 1.4142, 0.2, "loop() I_rms matches expected value");
  KNOWN_BUG(std::atof(o.c_str() + o.find("kwh= ") + 5) > 0, "loop() reports kwh > 0 under load (inner 'float power' shadows the accumulator)");
  SUMMARY();
}
