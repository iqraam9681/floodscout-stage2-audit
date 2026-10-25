# FloodScout ESP32-S3 Core Firmware

## Overview
This directory contains the embedded C++ firmware for the **FloodScout Autonomous Catamaran Survey Vessel**, target architecture **ESP32-S3 / ESP32 DevKit V1**.

The firmware performs real-time acoustic sounding (10 Hz), IMU attitude tilt compensation, GPS georeferencing, differential thruster motor control, and long-range RF telemetry broadcasting via ESP-NOW.

---

## File: `floodscout_esp32_firmware.ino`

### Key Features:
- **Acoustic Sounding**: Drives the JSN-SR04T waterproof transducer (or Blue Robotics Ping Sonar) with 10 µs trigger pulses and pulse-width echo timing adjusted for the speed of sound in water (~1498 m/s).
- **Trigonometric Tilt Correction**: Reads vessel roll ($\phi$) and pitch ($\theta$) from the MPU-6050 6-DOF IMU over Fast I2C (400 kHz) and applies geometric correction:
  $$\text{Depth}_{\text{corrected}} = \text{Depth}_{\text{raw}} \cdot \cos(\theta) \cdot \cos(\phi)$$
- **Autonomous Pothole Hazard Scoring**:
  - `0 (GREEN - SAFE)`: Water depth $< 10\text{ cm}$
  - `1 (AMBER - CAUTION)`: Water depth $10\text{ cm} \le d \le 25\text{ cm}$
  - `2 (RED - DANGER)`: Water depth $> 25\text{ cm}$ (submerged trench / open manhole)
- **High-Frequency Motor Control**: Uses ESP32 LEDC hardware PWM timer channels at 20 kHz to eliminate audible motor whine while providing linear thrust response to the TB6612FNG H-bridge.
- **ESP-NOW Ground Station Telemetry**: Transmits binary-packed telemetry packets every 100 ms to the handheld operator console up to 400 meters away without needing Wi-Fi routers or cellular connectivity.

---

## Compiling & Flashing Instructions

### Required Environment:
- **Arduino IDE v2.x** or **PlatformIO (VS Code)**
- **Board Package**: `esp32` by Espressif Systems (v2.0.14 or later)
- **Selected Board**: `ESP32S3 Dev Module` or `ESP32 Dev Module`

### Dependencies:
- Built-in ESP32 libraries: `Wire`, `WiFi`, `esp_now`

### Board Settings in Arduino IDE:
- **Upload Speed**: `921600`
- **CPU Frequency**: `240MHz (WiFi/BT)`
- **Flash Mode**: `QIO 80MHz`
- **Flash Size**: `4MB (32Mb)`
- **Partition Scheme**: `Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)`
- **Core Debug Level**: `None` (or `Info` for bench testing)
