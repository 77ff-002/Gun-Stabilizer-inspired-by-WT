#include "mpu6050.h"
#include <Wire.h>
#include "config.h"

bool MPU6050::begin(uint8_t addr) {
  _addr = addr;
  Wire.beginTransmission(_addr);
  if (Wire.endTransmission() != 0) return false;

  writeReg(0x6B, 0x00); // 唤醒
  delay(50);
  writeReg(0x19, 0x00); // 采样率分频 -> 1kHz
  writeReg(0x1A, 0x03); // DLPF：加速度 44Hz / 陀螺仪 42Hz
  writeReg(0x1B, 0x18); // 陀螺仪 ±2000 dps
  writeReg(0x1C, 0x00); // 加速度计 ±2g
  _lastUs = micros();
  return true;
}

void MPU6050::calibrate(uint16_t samples) {
  long gx = 0, gy = 0, gz = 0;
  for (uint16_t i = 0; i < samples; i++) {
    uint8_t buf[6];
    readBytes(0x43, buf, 6);
    gx += (int16_t)((buf[0] << 8) | buf[1]);
    gy += (int16_t)((buf[2] << 8) | buf[3]);
    gz += (int16_t)((buf[4] << 8) | buf[5]);
    delay(2);
  }
  _gxBias = (float)gx / samples;
  _gyBias = (float)gy / samples;
  _gzBias = (float)gz / samples;
}

void MPU6050::update() {
  uint8_t buf[6];

  readBytes(0x3B, buf, 6);
  int16_t ax = (int16_t)((buf[0] << 8) | buf[1]);
  int16_t ay = (int16_t)((buf[2] << 8) | buf[3]);
  int16_t az = (int16_t)((buf[4] << 8) | buf[5]);

  readBytes(0x43, buf, 6);
  int16_t gxr = (int16_t)((buf[0] << 8) | buf[1]);
  int16_t gyr = (int16_t)((buf[2] << 8) | buf[3]);
  int16_t gzr = (int16_t)((buf[4] << 8) | buf[5]);

  float axg = ax / 16384.0f;
  float ayg = ay / 16384.0f;
  float azg = az / 16384.0f;
  float gx = (gxr - _gxBias) / 16.4f;
  float gy = (gyr - _gyBias) / 16.4f;
  float gz = (gzr - _gzBias) / 16.4f;

  unsigned long now = micros();
  float dt = (_lastUs == 0) ? 0.0f : (float)(now - _lastUs) / 1000000.0f;
  _lastUs = now;
  if (dt <= 0.0f || dt > 0.1f) dt = 0.005f;

  float accPitch = atan2f(-axg, sqrtf(ayg * ayg + azg * azg)) * 57.2957795f;
  float accRoll = atan2f(ayg, azg) * 57.2957795f;

  pitchDeg = AHRS_ALPHA * (pitchDeg + gy * dt) + (1.0f - AHRS_ALPHA) * accPitch;
  rollDeg = AHRS_ALPHA * (rollDeg + gx * dt) + (1.0f - AHRS_ALPHA) * accRoll;

  pitchRateDps = gy;
  rollRateDps = gx;
  yawRateDps = gz;
}

void MPU6050::writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(_addr);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

void MPU6050::readBytes(uint8_t reg, uint8_t *buf, uint8_t len) {
  Wire.beginTransmission(_addr);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(_addr, len);
  uint8_t i = 0;
  while (Wire.available() && i < len) buf[i++] = Wire.read();
}
