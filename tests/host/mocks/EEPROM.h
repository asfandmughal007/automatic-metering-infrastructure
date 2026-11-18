#pragma once
#include "Arduino.h"
struct MockEEPROM {
  uint8_t mem[4096] = {0};
  void write(int a, uint8_t v) { mem[a] = v; }
  uint8_t read(int a) { return mem[a]; }
};
static MockEEPROM EEPROM;
