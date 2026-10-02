#include "servo_pwm.h"

void ServoPWM::attach(int pin) {
  _pin = pin;
  // ESP32-S3 的 LEDC 最高只支持 14bit，写成 16bit 会导致 ledcAttach 失败
  bool ok = ledcAttach((uint8_t)pin, 50, 14); // 50Hz, 14bit
  if (!ok) {
    Serial.print("[servo] ledcAttach FAILED on pin ");
    Serial.println(pin);
  } else {
    Serial.print("[servo] attached pin ");
    Serial.println(pin);
  }
}

void ServoPWM::writeMicroseconds(uint16_t us) {
  if (_pin < 0) return;
  uint16_t clamped = constrain(us, (uint16_t)500, (uint16_t)2500);
  uint32_t duty = (uint32_t)((uint64_t)clamped * 16384 / 20000);
  ledcWrite((uint8_t)_pin, duty);
}
