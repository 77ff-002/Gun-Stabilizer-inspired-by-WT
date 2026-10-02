#include "stabilizer.h"

void Stabilizer::begin() {
  _elev.attach(PIN_SERVO_ELEVATION);
  _trav.attach(PIN_SERVO_TRAVERSE);
  if (AUX_SERVO_ENABLED) _aux.attach(PIN_SERVO_AUX);

  _elev.writeMicroseconds(PULSE_CENTER_US);
  _trav.writeMicroseconds(PULSE_CENTER_US);
  if (AUX_SERVO_ENABLED) _aux.writeMicroseconds(PULSE_CENTER_US);

#if SERVO_TEST_ON_BOOT
  delay(400);
  // 360° 连续舵机：先正转再反转
  _trav.writeMicroseconds(PULSE_CENTER_US + 400); // 1900us
  delay(500);
  _trav.writeMicroseconds(PULSE_CENTER_US - 400); // 1100us
  delay(500);
  _trav.writeMicroseconds(PULSE_CENTER_US);
  delay(300);
  // 180° 舵机：扫到两头再回中
  _elev.writeMicroseconds(800);
  delay(400);
  _elev.writeMicroseconds(2200);
  delay(400);
  _elev.writeMicroseconds(PULSE_CENTER_US);
  delay(200);
#endif
}

void Stabilizer::update(float hullPitchDeg, float hullYawRateDps, float dtSec) {
  // ---- 俯仰：保持惯性系仰角 ----
  float comp = _enabled ? (ELEV_STAB_INVERT * ELEV_STAB_GAIN * hullPitchDeg) : 0.0f;
  float servoElevDeg = _elevCmd + comp;
  float pulse = PULSE_CENTER_US + servoElevDeg * ELEV_US_PER_DEG;
  pulse = constrain(pulse, (float)PULSE_MIN_US, (float)PULSE_MAX_US);
  _elevPulse = (uint16_t)pulse;

  // ---- 方位：角度舵机（位置控制）----
  // 操作者转向速率 + 抵消车体横摆角速度，积分成炮塔角度
  float yawComp = _enabled ? (YAW_STAB_INVERT * YAW_STAB_GAIN * hullYawRateDps) : 0.0f;
  _travAngleDeg += (_travCmd + yawComp) * dtSec;
  _travAngleDeg = constrain(_travAngleDeg, -TRAVERSE_MAX_DEG, TRAVERSE_MAX_DEG);
  float pos = _travAngleDeg / TRAVERSE_MAX_DEG;
  float tpulse = PULSE_CENTER_US + pos * TRAVERSE_PULSE_SPAN;
  _travPulse = (uint16_t)constrain(tpulse, (float)PULSE_MIN_US, (float)PULSE_MAX_US);

  _elev.writeMicroseconds(_elevPulse);
  _trav.writeMicroseconds(_travPulse);

  if (AUX_SERVO_ENABLED) {
    uint16_t aux = (AUX_INVERT > 0.0f)
                       ? _elevPulse
                       : (uint16_t)(2 * PULSE_CENTER_US - _elevPulse);
    _aux.writeMicroseconds(constrain(aux, (uint16_t)PULSE_MIN_US, (uint16_t)PULSE_MAX_US));
  }
}

void Stabilizer::setElevationCommand(float deg) {
  _elevCmd = constrain(deg, ELEV_MIN_DEG, ELEV_MAX_DEG);
}

void Stabilizer::setTraverseCommand(float dps) {
  _travCmd = constrain(dps, -TRAVERSE_MAX_DPS, TRAVERSE_MAX_DPS);
}

void Stabilizer::setTraverseAngle(float deg) {
  _travAngleDeg = constrain(deg, -TRAVERSE_MAX_DEG, TRAVERSE_MAX_DEG);
  _travCmd = 0.0f;
}

void Stabilizer::setEnabled(bool en) { _enabled = en; }

void Stabilizer::setMode(ControlMode m) { _mode = m; }
