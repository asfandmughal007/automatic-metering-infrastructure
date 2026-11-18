#pragma once
#include "Arduino.h"
struct LiquidCrystal {
  std::string last;
  LiquidCrystal(int, int, int, int, int, int) {}
  void begin(int, int) {}
  void clear() { last.clear(); }
  void setCursor(int, int) {}
  void print(const char* s) { last += s; }
  template <class T> void print(T v) { last += std::to_string(v); }
};
