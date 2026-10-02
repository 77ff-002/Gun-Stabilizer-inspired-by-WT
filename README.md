# 简陋的火炮稳定器（ESP32-S3，Arduino IDE）

基于 ESP32-S3 + MPU6050 + ELRS 2.4G nano 的两轴火炮稳定器，模拟现代坦克的火炮稳定，支持遥控和 WiFi 网页两种控制方式。

引脚

| 信号 | ESP32-S3 引脚 |
| --- | --- |
| MPU6050 SDA | GPIO8 |
| MPU6050 SCL | GPIO9 |
| ELRS 接收机 TX | GPIO16（ESP32 RX） |
| 360° 舵机信号 | GPIO4 |
| 180° 舵机信号 | GPIO5 |

供电

- MPU6050：VCC 接 3.3V 或 5V（GY-521 板载稳压，读不到就换 5V），GND 共地。
- ELRS 接收机：5V，GND 共地。
- 舵机：用独立 5V 电源（舵机吃电大，别从 ESP32 取电），GND 与 ESP32 共地。

 
控制

默认ch1 和ch2

WiFi 网页控制
 
手机连 WiFi `GunStabilizer`，密码 `12345678`。
浏览器打开 `http://192.168.4.1`。

所有引脚、通道、稳定方向、行程、WiFi 名称密码都集中在 `GunStabilizer/config.h` 顶部：

- 补偿方向反了：改 `ELEV_STAB_INVERT` / `YAW_STAB_INVERT` 的符号。
- 炮塔摆角不合适：改 `TRAVERSE_PULSE_SPAN`（500～1000 之间试）。
- 炮塔转速快慢：改 `TRAVERSE_MAX_DPS`。
- 上电自检：调好后把 `SERVO_TEST_ON_BOOT` 改成 0 关闭。
