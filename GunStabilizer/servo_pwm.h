#pragma once
#include <Arduino.h>

class ServoPWM {
public:
  void attach(int pin);
  void writeMicroseconds(uint16_t us);

private:
  int _pin = -1;
};
