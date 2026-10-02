#pragma once
#include <Arduino.h>

// 简单 MPU6050 驱动 + 互补滤波姿态解算
// 安装约定：芯片平放，X 轴朝车体前方，Z 轴朝上。
class MPU6050 {
public:
  bool begin(uint8_t addr = 0x68);
  void calibrate(uint16_t samples = 500); // 静止时调用，标定陀螺仪零偏
  void update();                          // 高频调用（建议 200Hz）

  float pitchDeg = 0.0f;      // 俯仰角（车头翘起为正）
  float rollDeg = 0.0f;       // 横滚角
  float pitchRateDps = 0.0f;  // 俯仰角速度 deg/s
  float rollRateDps = 0.0f;   // 横滚角速度 deg/s
  float yawRateDps = 0.0f;    // 横摆（航向）角速度 deg/s

private:
  uint8_t _addr = 0x68;
  float _gxBias = 0.0f, _gyBias = 0.0f, _gzBias = 0.0f;
  unsigned long _lastUs = 0;
  void writeReg(uint8_t reg, uint8_t val);
  void readBytes(uint8_t reg, uint8_t *buf, uint8_t len);
};
