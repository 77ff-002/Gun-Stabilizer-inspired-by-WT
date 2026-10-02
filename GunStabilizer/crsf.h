#pragma once
#include <Arduino.h>

// ELRS 接收机 CRSF 协议解析（只解析 RC 通道帧，type 0x16）
class CRSF {
public:
  void begin(HardwareSerial &s, int8_t rxPin, int8_t txPin);
  void process(); // 在 loop 中轮询
  bool isAlive() const { return _hasFrame && (millis() - _lastFrameMs) < 1000UL; }
  const uint16_t *channels() const { return _ch; }
  uint32_t rxBytes() const { return _rxBytes; }

private:
  HardwareSerial *_ser = nullptr;
  uint16_t _ch[16] = {0};
  unsigned long _lastFrameMs = 0;
  bool _hasFrame = false;
  uint32_t _rxBytes = 0;
  uint8_t _buf[64];
  uint8_t _idx = 0;
  uint8_t _frameLen = 0;
  uint8_t crc8(uint8_t crc, uint8_t a);
  void parseRc(const uint8_t *p);
};
