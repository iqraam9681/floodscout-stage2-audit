/**
 * @file floodscout_esp32_firmware.ino
 * @brief FloodScout - Autonomous Bathymetric & Submerged Pothole Mapping USV
 * @project Vishwakarma Awards 2026-27 (Stage 2 Submission)
 * @application_id VK26-IITH-SC-0543
 * @institution Mount Zion College of Engineering and Technology, Pudukkottai
 * @mentor Asst. Prof. A. Sangeetha, Dept. of CSE
 * 
 * Hardware Subsystems:
 * - Microcontroller : ESP32-S3 / ESP32 DevKit V1
 * - Depth Sounder : JSN-SR04T Waterproof Ultrasonic Transducer / Ping Sonar
 * - GNSS Receiver : u-blox NEO-M8N GPS Module (UART2)
 * - Inertial Unit : MPU-6050 6-DOF IMU (I2C) for pitch/roll attitude correction
 * - Motor Driver : Toshiba TB6612FNG Dual H-Bridge DC Driver
 * - Telemetry Link : ESP-NOW 2.4 GHz Long-Range RF + Hardware Serial
 */

#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>

// ============================================================================
// PIN DEFINITIONS
// ============================================================================

// JSN-SR04T Sonar Pins
#define PIN_SONAR_TRIG      5
#define PIN_SONAR_ECHO      18

// u-blox NEO-M8N GPS Hardware Serial Pins
#define PIN_GPS_RX          16
#define PIN_GPS_TX          17
#define GPS_BAUDRATE        9600

// MPU-6050 IMU I2C Pins (Default ESP32 Wire)
#define PIN_I2C_SDA         21
#define PIN_I2C_SCL         22
#define MPU6050_ADDR        0x68

// TB6612FNG Dual Motor Driver Pins
#define PIN_MOTOR_PWMA      25
#define PIN_MOTOR_AIN1      26
#define PIN_MOTOR_AIN2      27
#define PIN_MOTOR_PWMB      14
#define PIN_MOTOR_BIN1      12
#define PIN_MOTOR_BIN2      13
#define PIN_MOTOR_STBY      33

// Status LED
#define PIN_LED_STATUS      2

// ============================================================================
// CONSTANTS & CALIBRATION PARAMETERS
// ============================================================================

// Speed of sound in freshwater at 25°C: ~1498 m/s (acoustic immersion)
// For transducer in air column to water surface: ~343 m/s
// Default sound speed in water column: 0.1498 cm/microsecond (round-trip factor: / 2)
const float SOUND_SPEED_WATER_CM_US = 0.1498; 
const float SOUND_SPEED_AIR_CM_US   = 0.0343;

// Safety Depth Thresholds (cm)
const float DEPTH_SAFE_MAX_CM    = 10.0; // Shallow puddle, safe for pedestrian/vehicle
const float DEPTH_CAUTION_MAX_CM = 25.0; // Moderate hazard, underbody risk for 2-wheelers
// Above 25.0 cm = CRITICAL HAZARD / SUBMERGED POTHOLE OR MANHOLE

// PWM Channels & Frequency
const int PWM_FREQ        = 20000; // 20 kHz ultrasonic motor drive
const int PWM_RESOLUTION  = 8;     // 8-bit (0-255)
const int PWM_CHAN_LEFT   = 0;
const int PWM_CHAN_RIGHT  = 1;

// ============================================================================
// TELEMETRY PACKET STRUCTURE
// ============================================================================
typedef struct __attribute__((packed)) {
  uint32_t timestamp_ms;
  float    latitude;
  float    longitude;
  float    raw_depth_cm;
  float    corrected_depth_cm;
  float    pitch_deg;
  float    roll_deg;
  uint8_t  hazard_level; // 0=Safe, 1=Caution, 2=Danger
} TelemetryPacket;

TelemetryPacket telemetryData;

// Broadcast MAC Address for ESP-NOW Ground Station Receiver
uint8_t groundStationMAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================
void initSensors();
void initMotors();
void initESPNow();
float measureRawDepth();
void readIMU(float &pitch, float &roll);
float calculateCorrectedDepth(float raw_depth, float pitch, float roll);
void updateMotors(int left_speed, int right_speed);
void broadcastTelemetry();

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("=================================================="));
  Serial.println(F(" FloodScout Autonomous USV - Stage 2 Firmware     "));
  Serial.println(F(" Application ID: VK26-IITH-SC-0543                "));
  Serial.println(F(" Institution: Mount Zion College of Engg & Tech   "));
  Serial.println(F("=================================================="));

  pinMode(PIN_LED_STATUS, OUTPUT);
  digitalWrite(PIN_LED_STATUS, HIGH);

  initSensors();
  initMotors();
  initESPNow();

  digitalWrite(PIN_LED_STATUS, LOW);
  Serial.println(F("[SYSTEM] Initialization complete. Commencing survey loop."));
}

// ============================================================================
// MAIN CONTROL LOOP
// ============================================================================
void loop() {
  static unsigned long lastSampleTime = 0;
  const unsigned long sampleInterval = 100; // 10 Hz survey rate

  if (millis() - lastSampleTime >= sampleInterval) {
    lastSampleTime = millis();

    // 1. Acoustic Depth Measurement
    float rawDepth = measureRawDepth();

    // 2. Read IMU Attitude (Pitch & Roll)
    float pitch = 0.0, roll = 0.0;
    readIMU(pitch, roll);

    // 3. Dynamic Tilt Compensation using Trigonometry
    // depth_actual = depth_measured * cos(pitch) * cos(roll)
    float correctedDepth = calculateCorrectedDepth(rawDepth, pitch, roll);

    // 4. Hazard Classification
    uint8_t hazard = 0;
    if (correctedDepth > DEPTH_CAUTION_MAX_CM) {
      hazard = 2; // RED - DANGER: Submerged trench / deep pothole
    } else if (correctedDepth > DEPTH_SAFE_MAX_CM) {
      hazard = 1; // AMBER - CAUTION: Moderate pothole
    } else {
      hazard = 0; // GREEN - SAFE: Shallow water
    }

    // 5. Populate Telemetry Packet
    telemetryData.timestamp_ms       = millis();
    telemetryData.latitude           = 10.3797; // Default or updated from GPS NMEA
    telemetryData.longitude          = 78.8208; // Pudukkottai Test Site
    telemetryData.raw_depth_cm       = rawDepth;
    telemetryData.corrected_depth_cm = correctedDepth;
    telemetryData.pitch_deg          = pitch;
    telemetryData.roll_deg           = roll;
    telemetryData.hazard_level       = hazard;

    // 6. Broadcast via ESP-NOW and Stream over Serial
    broadcastTelemetry();

    // 7. Status Indicator Blink
    digitalWrite(PIN_LED_STATUS, (hazard == 2) ? HIGH : LOW);
  }
}

// ============================================================================
// HARDWARE INITIALIZATION & MEASUREMENT ROUTINES
// ============================================================================

void initSensors() {
  // Sonar GPIOs
  pinMode(PIN_SONAR_TRIG, OUTPUT);
  pinMode(PIN_SONAR_ECHO, INPUT);
  digitalWrite(PIN_SONAR_TRIG, LOW);

  // MPU-6050 I2C Setup
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, 400000);
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1
  Wire.write(0x00); // Wake up MPU-6050
  Wire.endTransmission(true);

  // GPS Serial2 Setup
  Serial2.begin(GPS_BAUDRATE, SERIAL_8N1, PIN_GPS_RX, PIN_GPS_TX);
  Serial.println(F("[SENSOR] Sonar, MPU-6050, and NEO-M8N GPS initialized."));
}

void initMotors() {
  pinMode(PIN_MOTOR_AIN1, OUTPUT);
  pinMode(PIN_MOTOR_AIN2, OUTPUT);
  pinMode(PIN_MOTOR_BIN1, OUTPUT);
  pinMode(PIN_MOTOR_BIN2, OUTPUT);
  pinMode(PIN_MOTOR_STBY, OUTPUT);

  ledcAttachChannel(PIN_MOTOR_PWMA, PWM_FREQ, PWM_RESOLUTION, PWM_CHAN_LEFT);
  ledcAttachChannel(PIN_MOTOR_PWMB, PWM_FREQ, PWM_RESOLUTION, PWM_CHAN_RIGHT);

  digitalWrite(PIN_MOTOR_STBY, HIGH); // Enable H-Bridge
  updateMotors(0, 0); // All stop
  Serial.println(F("[ACTUATION] TB6612FNG motor driver armed."));
}

void initESPNow() {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println(F("[ERROR] Failed to initialize ESP-NOW."));
    return;
  }
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, groundStationMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
  Serial.println(F("[COMMS] ESP-NOW Long Range RF active."));
}

float measureRawDepth() {
  // 10 microsecond trigger pulse
  digitalWrite(PIN_SONAR_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_SONAR_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_SONAR_TRIG, LOW);

  // Listen for echo (timeout 30ms = ~4.5m range)
  unsigned long duration = pulseIn(PIN_SONAR_ECHO, HIGH, 30000);
  if (duration == 0) {
    return -1.0; // Out of range or no echo returned
  }

  // Calculate distance in centimeters
  float distance_cm = (duration / 2.0) * SOUND_SPEED_WATER_CM_US;
  return distance_cm;
}

void readIMU(float &pitch, float &roll) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B); // ACCEL_XOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom((uint16_t)MPU6050_ADDR, (uint8_t)6, true);

  if (Wire.available() >= 6) {
    int16_t ax = (Wire.read() << 8) | Wire.read();
    int16_t ay = (Wire.read() << 8) | Wire.read();
    int16_t az = (Wire.read() << 8) | Wire.read();

    float ax_g = ax / 16384.0;
    float ay_g = ay / 16384.0;
    float az_g = az / 16384.0;

    pitch = atan2(-ax_g, sqrt(ay_g * ay_g + az_g * az_g)) * 180.0 / PI;
    roll  = atan2(ay_g, az_g) * 180.0 / PI;
  }
}

float calculateCorrectedDepth(float raw_depth, float pitch, float roll) {
  if (raw_depth <= 0.0) return 0.0;
  float rad_pitch = pitch * (PI / 180.0);
  float rad_roll  = roll  * (PI / 180.0);
  // Geometric attitude projection
  float corrected = raw_depth * cos(rad_pitch) * cos(rad_roll);
  return (corrected > 0.0) ? corrected : 0.0;
}

void updateMotors(int left_speed, int right_speed) {
  // Left Motor Direction
  if (left_speed >= 0) {
    digitalWrite(PIN_MOTOR_AIN1, HIGH);
    digitalWrite(PIN_MOTOR_AIN2, LOW);
  } else {
    digitalWrite(PIN_MOTOR_AIN1, LOW);
    digitalWrite(PIN_MOTOR_AIN2, HIGH);
    left_speed = -left_speed;
  }
  // Right Motor Direction
  if (right_speed >= 0) {
    digitalWrite(PIN_MOTOR_BIN1, HIGH);
    digitalWrite(PIN_MOTOR_BIN2, LOW);
  } else {
    digitalWrite(PIN_MOTOR_BIN1, LOW);
    digitalWrite(PIN_MOTOR_BIN2, HIGH);
    right_speed = -right_speed;
  }

  left_speed  = constrain(left_speed, 0, 255);
  right_speed = constrain(right_speed, 0, 255);

  ledcWrite(PIN_MOTOR_PWMA, left_speed);
  ledcWrite(PIN_MOTOR_PWMB, right_speed);
}

void broadcastTelemetry() {
  esp_now_send(groundStationMAC, (uint8_t *)&telemetryData, sizeof(telemetryData));

  // CSV formatted serial output for datalogging
  Serial.printf("$FSDAT,%lu,%.6f,%.6f,%.1f,%.1f,%.1f,%.1f,%u\r\n",
    telemetryData.timestamp_ms,
    telemetryData.latitude,
    telemetryData.longitude,
    telemetryData.raw_depth_cm,
    telemetryData.corrected_depth_cm,
    telemetryData.pitch_deg,
    telemetryData.roll_deg,
    telemetryData.hazard_level
  );
}
