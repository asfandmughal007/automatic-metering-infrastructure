// Minimal host-side stand-in for the Arduino core, just enough to compile and exercise the
// AMI sketches on a PC. A virtual clock advances on every ADC read (like a real AVR ADC), and
// the analog/digital inputs are driven by a configurable single-phase sine-wave simulation.
#pragma once
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define INPUT 0
#define OUTPUT 1
#define HIGH 1
#define LOW 0
#define RISING 3

// Arduino Mega pin numbering
enum { A0 = 54, A1, A2, A3, A4, A5 };

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

// ---------------- simulated board state ----------------
struct SimBoard {
  double now_us = 0;                // virtual clock
  double adc_us = 112;              // one analogRead() on a 16 MHz AVR (prescaler 128)
  double mains_hz = 50;
  double v_offset = 512, v_peak = 300;   // ADC counts
  double i_offset = 516, i_peak = 100;   // ADC counts (516 is the ACS712 zero used in the sketches)
  double phase_deg = 0;             // current lags voltage by this angle
  double watchdog_us = 3.6e9 + 1e9; // abort runaway busy-wait loops
  double v_wave(double t_us) const { return std::sin(2 * M_PI * mains_hz * t_us * 1e-6); }
  double i_wave(double t_us) const {
    return std::sin(2 * M_PI * mains_hz * t_us * 1e-6 - phase_deg * M_PI / 180.0);
  }
};
inline SimBoard& sim() { static SimBoard s; return s; }
inline void sim_reset() { sim() = SimBoard(); }

inline void check_watchdog() {
  if (sim().now_us > sim().watchdog_us) throw std::runtime_error("virtual-time watchdog: busy-wait never ended");
}

inline unsigned long millis() { return (unsigned long)(sim().now_us / 1000.0); }
inline void delay(unsigned long ms) { sim().now_us += ms * 1000.0; }

inline int analogRead(int pin) {
  SimBoard& s = sim();
  s.now_us += s.adc_us;
  check_watchdog();
  double v;
  if (pin == A1) v = s.i_offset + s.i_peak * s.i_wave(s.now_us);   // current sensor
  else v = s.v_offset + s.v_peak * s.v_wave(s.now_us);             // voltage sensor
  v = std::round(v);
  return v < 0 ? 0 : (v > 1023 ? 1023 : (int)v);
}
inline int digitalRead(int) {            // zero-crossing / frequency input: square wave in phase with voltage
  SimBoard& s = sim();
  s.now_us += 4;                          // digitalRead is fast (~4 us)
  check_watchdog();
  return s.v_wave(s.now_us) > 0 ? HIGH : LOW;
}
inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}
inline void attachInterrupt(int, void (*)(), int) {}
inline void detachInterrupt(int) {}
static volatile int EIFR = 0;

// ---------------- Serial ----------------
struct MockSerial {
  std::string out;
  bool echo = false;
  void begin(long) {}
  void flush() {}
  void emit(const std::string& s) { out += s; if (echo) std::fputs(s.c_str(), stdout); }
  template <class T> void print(T v) { emit(std::to_string(v)); }
  void print(float v) { char b[48]; std::snprintf(b, sizeof b, "%.2f", v); emit(b); }
  void print(double v) { char b[48]; std::snprintf(b, sizeof b, "%.2f", v); emit(b); }
  void print(const char* s) { emit(s); }
  template <class T> void println(T v) { print(v); emit("\n"); }
  void println(const char* s) { emit(s); emit("\n"); }
  void println() { emit("\n"); }
};
static MockSerial Serial, Serial2, Serial3;
