#include "crsf.h"
#include "config.h"

void CRSF::begin(HardwareSerial &s, int8_t rxPin, int8_t txPin) {
  _ser = &s;
  // ELRS 接收机 CRSF 串口为反相信号，用 begin 的 invert 参数统一处理
  s.begin(420000, SERIAL_8N1, rxPin, txPin, ELRS_INVERT_RX ? true : false);
}

void CRSF::process() {
  if (!_ser) return;
  while (_ser->available()) {
    uint8_t b = _ser->read();
    _rxBytes++;
    if (_idx == 0) {
      if (b == 0xC8) _buf[_idx++] = b;
    } else if (_idx == 1) {
      if (b <= 62) {
        _buf[_idx++] = b;
        _frameLen = b + 2;
      } else {
        _idx = 0;
      }
    } else {
      _buf[_idx++] = b;
      if (_idx >= _frameLen) {
        uint8_t crc = 0;
        for (uint8_t i = 2; i < _frameLen - 1; i++) crc = crc8(crc, _buf[i]);
        if (crc == _buf[_frameLen - 1]) {
          if (_buf[2] == 0x16) parseRc(&_buf[3]);
          _lastFrameMs = millis();
          _hasFrame = true;
        }
        _idx = 0;
      }
    }
  }
}

void CRSF::parseRc(const uint8_t *p) {
  for (int i = 0; i < 16; i++) {
    uint32_t bit = (uint32_t)i * 11;
    uint32_t byte = bit / 8;
    uint32_t shift = bit % 8;
    uint16_t v = (uint16_t)((p[byte] | (p[byte + 1] << 8)) >> shift);
    if (shift > 5) v |= (uint16_t)(p[byte + 2] << (16 - shift));
    _ch[i] = v & 0x7FF;
  }
}

uint8_t CRSF::crc8(uint8_t crc, uint8_t a) {
  crc ^= a;
  for (int i = 0; i < 8; i++) {
    crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0xD5) : (uint8_t)(crc << 1);
  }
  return crc;
}
