#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "config.h"
#include "mpu6050.h"
#include "crsf.h"
#include "stabilizer.h"
#include "web_ui.h"

MPU6050 mpu;
CRSF crsf;
Stabilizer stab;
WebServer server(80);
bool mpuReady = false;

float mapRc(uint16_t raw) {
  float v = (raw - 992.0f) / 819.0f; // CRSF 172..1811，中位 992
  if (fabsf(v) < RC_DEADBAND) v = 0.0f;
  return constrain(v, -1.0f, 1.0f);
}

void updateRcCommands() {
  if (stab.mode() != MODE_RC) return;
  if (!crsf.isAlive()) {
    stab.setTraverseCommand(0.0f); // 接收机离线时停转，俯仰保持
    return;
  }
  const uint16_t *ch = crsf.channels();
  float ev = mapRc(ch[CH_ELEVATION]);
  float tv = mapRc(ch[CH_TRAVERSE]);
  float elev = (ev >= 0.0f) ? ev * ELEV_MAX_DEG : ev * (-ELEV_MIN_DEG);
  stab.setElevationCommand(elev);
  stab.setTraverseCommand(tv * TRAVERSE_MAX_DPS);
}

void scanI2C() {
  Serial.println("[i2c] scanning bus...");
  uint8_t found = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("[i2c] device at 0x");
      if (addr < 0x10) Serial.print("0");
      Serial.println(addr, HEX);
      found++;
    }
  }
  if (found == 0) Serial.println("[i2c] no device found");
}

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void handleState() {
  String s = "{";
  s += "\"pitch\":" + String(mpu.pitchDeg, 2);
  s += ",\"roll\":" + String(mpu.rollDeg, 2);
  s += ",\"yaw\":" + String(mpu.yawRateDps, 2);
  s += ",\"elevCmd\":" + String(stab.elevationCommand(), 1);
  s += ",\"travCmd\":" + String(stab.traverseCommand(), 1);
  s += ",\"elevPulse\":" + String((unsigned int)stab.elevationPulse());
  s += ",\"travPulse\":" + String((unsigned int)stab.traversePulse());
  s += ",\"mode\":\"" + String(stab.mode() == MODE_WIFI ? "WIFI" : "RC") + "\"";
  s += ",\"rc\":" + String(crsf.isAlive() ? 1 : 0);
  s += ",\"stab\":" + String(stab.enabled() ? 1 : 0);
  s += "}";
  server.send(200, "application/json", s);
}

void handleSet() {
  if (server.hasArg("e")) stab.setElevationCommand(server.arg("e").toFloat());
  if (server.hasArg("t")) stab.setTraverseAngle(server.arg("t").toFloat());
  if (server.hasArg("stab")) stab.setEnabled(server.arg("stab") == "1");
  if (server.hasArg("mode")) stab.setMode(server.arg("mode") == "wifi" ? MODE_WIFI : MODE_RC);
  server.send(200, "application/json", "{\"ok\":1}");
}

void handleNotFound() {
  server.send(404, "text/plain", "404");
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println();
  Serial.println("[stabilizer] boot");

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, 400000);
  scanI2C();

  bool mpuOk = mpu.begin(0x68);
  uint8_t mpuAddr = 0x68;
  if (!mpuOk) {
    mpuOk = mpu.begin(0x69);
    mpuAddr = 0x69;
  }
  mpuReady = mpuOk;
  if (mpuOk) {
    Serial.print("[mpu6050] found at 0x");
    Serial.println(mpuAddr, HEX);
    Serial.println("[mpu6050] calibrating, keep still...");
    mpu.calibrate(500);
    Serial.println("[mpu6050] calibrated");
  } else {
    Serial.println("[mpu6050] NOT found: stabilization disabled");
    Serial.println("[mpu6050] check VCC=5V, GND, SDA->GPIO8, SCL->GPIO9");
  }

  crsf.begin(Serial2, PIN_ELRS_RX, PIN_ELRS_TX);
  stab.begin();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD, WIFI_AP_CHANNEL, WIFI_AP_HIDDEN, 4);
  Serial.print("[wifi] AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/state", handleState);
  server.on("/set", handleSet);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("[web] server started");
}

void loop() {
  crsf.process();

  static unsigned long lastRc = 0;
  if (millis() - lastRc >= 20) {
    lastRc = millis();
    updateRcCommands();
  }

  static unsigned long lastCtrl = 0;
  unsigned long nowCtrl = millis();
  if (nowCtrl - lastCtrl >= 5) {
    float dtSec = (nowCtrl - lastCtrl) / 1000.0f;
    lastCtrl = nowCtrl;
    if (dtSec <= 0.0f || dtSec > 0.1f) dtSec = 0.005f;
    float pitch = 0.0f, yawRate = 0.0f;
    if (mpuReady) {
      mpu.update();
      pitch = mpu.pitchDeg;
      yawRate = mpu.yawRateDps;
    }
    stab.update(pitch, yawRate, dtSec);
  }

  server.handleClient();

  static unsigned long lastDbg = 0;
  static uint32_t lastRx = 0;
  if (millis() - lastDbg >= 1000) {
    lastDbg = millis();
    uint32_t rxNow = crsf.rxBytes();
    uint32_t rxDelta = rxNow - lastRx;
    lastRx = rxNow;
    const uint16_t *ch = crsf.channels();
    String line = "pitch=" + String(mpu.pitchDeg, 1);
    line += " yaw=" + String(mpu.yawRateDps, 1);
    line += " | elev=" + String(stab.elevationCommand(), 1);
    line += " trav=" + String(stab.traverseCommand(), 1);
    line += " travA=" + String(stab.traverseAngle(), 1);
    line += " | chE=" + String((unsigned int)ch[CH_ELEVATION]);
    line += " chT=" + String((unsigned int)ch[CH_TRAVERSE]);
    line += " | elevP=" + String((unsigned int)stab.elevationPulse());
    line += " travP=" + String((unsigned int)stab.traversePulse());
    line += " | mode=" + String(stab.mode() == MODE_WIFI ? "WIFI" : "RC");
    line += " rx=" + String((unsigned long)rxDelta);
    line += " rc=" + String(crsf.isAlive() ? "OK" : "LOST");
    Serial.println(line);
  }
}
