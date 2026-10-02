#pragma once
#include <Arduino.h>
#include "config.h"
#include "servo_pwm.h"

enum ControlMode : uint8_t { MODE_RC = 0, MODE_WIFI = 1 };

class Stabilizer {
public:
  void begin();
  void update(float hullPitchDeg, float hullYawRateDps, float dtSec);

  void setElevationCommand(float deg);
  void setTraverseCommand(float dps);   // RC：转向速率，内部积分成角度
  void setTraverseAngle(float deg);     // WiFi：直接设置炮塔角度
  void setEnabled(bool en);
  void setMode(ControlMode m);

  bool enabled() const { return _enabled; }
  ControlMode mode() const { return _mode; }
  float elevationCommand() const { return _elevCmd; }
  float traverseCommand() const { return _travCmd; }
  float traverseAngle() const { return _travAngleDeg; }
  uint16_t elevationPulse() const { return _elevPulse; }
  uint16_t traversePulse() const { return _travPulse; }

private:
  bool _enabled = true;
  ControlMode _mode = MODE_RC;
  float _elevCmd = 0.0f;
  float _travCmd = 0.0f;       // 转向速率指令 deg/s
  float _travAngleDeg = 0.0f;  // 炮塔当前角度 deg
  uint16_t _elevPulse = PULSE_CENTER_US;
  uint16_t _travPulse = PULSE_CENTER_US;
  ServoPWM _elev, _trav, _aux;
};
