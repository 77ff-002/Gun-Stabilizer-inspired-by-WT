#pragma once

// ===================== 引脚定义（按你的实际接线修改） =====================
#define PIN_I2C_SDA           8     // MPU6050 SDA
#define PIN_I2C_SCL           9     // MPU6050 SCL

#define PIN_ELRS_RX           16    // ELRS 接收机 TX -> ESP32 RX
#define PIN_ELRS_TX           17    // ESP32 TX -> ELRS RX（本工程暂不发送遥测）

#define PIN_SERVO_TRAVERSE    4     // 360° 角度舵机（位置控制）：炮塔方位（横摆）
#define PIN_SERVO_ELEVATION   5     // 180° 舵机：火炮俯仰
#define PIN_SERVO_AUX         6     // 可选第二个 180° 舵机

// ===================== ELRS / CRSF =====================
#define ELRS_UART_NUM         2     // Serial2 对应 UART_NUM_2
#define ELRS_INVERT_RX        0     // 接收机串口是否反相；收不到数据就换 1 试试
#define CH_ELEVATION          1     // CH2 -> 180° 俯仰舵机（0 基下标：CH1=0）
#define CH_TRAVERSE           0     // CH1 -> 360° 转向舵机
#define RC_DEADBAND           0.03f // 摇杆死区

// ===================== MPU6050 =====================
#define MPU_ADDR              0x68  // I2C 地址（AD0 接地）
#define AHRS_ALPHA            0.98f // 互补滤波陀螺仪权重

// ===================== 舵机 PWM =====================
#define SERVO_FREQ_HZ         50
#define PULSE_MIN_US          500
#define PULSE_MAX_US          2500
#define PULSE_CENTER_US       1500
#define ELEV_US_PER_DEG       11.1111f // (2500-500)/180：每度对应的脉宽微秒数

// 俯仰指令范围（惯性系角度，度）
#define ELEV_MIN_DEG          -30.0f
#define ELEV_MAX_DEG          60.0f
#define ELEV_NEUTRAL_DEG      0.0f

// 转向（横摆）——角度舵机，位置控制
#define TRAVERSE_MAX_DPS      180.0f  // 满杆对应的转向速率 deg/s（RC 摇杆）
#define TRAVERSE_MAX_DEG      170.0f  // 炮塔最大偏转角度（逻辑值，度）
#define TRAVERSE_PULSE_SPAN   900.0f  // 最大角度时偏离中位(1500us)的脉宽 -> 600~2400us

// ===================== 稳定器 =====================
// 若某轴补偿方向反了，把对应的 INVERT 从 -1.0 改成 +1.0
#define ELEV_STAB_INVERT      -1.0f
#define YAW_STAB_INVERT       -1.0f
#define ELEV_STAB_GAIN        1.0f
#define YAW_STAB_GAIN         1.0f

// ===================== 辅助舵机（第二个 180°） =====================
#define AUX_SERVO_ENABLED     0
#define AUX_INVERT            -1.0f // +1 与俯仰同向，-1 与俯仰反向

// ===================== 上电舵机自检 =====================
// 1 = 开机时让舵机动一下，方便确认接线/供电；调好后可改成 0
#define SERVO_TEST_ON_BOOT    1

// ===================== WiFi 热点 =====================
#define WIFI_AP_SSID          "GunStabilizer"
#define WIFI_AP_PASSWORD      "12345678" // WPA2 至少 8 位
#define WIFI_AP_CHANNEL       6
#define WIFI_AP_HIDDEN        0
