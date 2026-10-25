# Hardware Interconnect & Wiring Specification

## Overview
This document details the wiring and electrical schematic of the **FloodScout Autonomous Catamaran Survey Vessel**, corresponding to the visual circuit diagram `circuit_diagram_components.jpg`.

![FloodScout Visual Circuit Diagram](circuit_diagram_components.jpg)

---

## Central Controller: ESP32 Development Board (ESP32-WROOM-32U / ESP32-S3)
- **Logic Voltage**: 3.3V
- **Supply Voltage**: 5.0V regulated via `VIN` pin

---

## Pinout & Connection Matrix

### 1. Acoustic Depth Sensor (Blue Robotics Ping2 Acoustic Echosounder)
The waterproof transducer probe is mounted submerged or through-hull on the catamaran central keel, facing straight down into the roadbed.
| Ping2 Sonar Pin | ESP32 Pin | Function / Wire Color | Notes |
|:---|:---|:---|:---|
| **VCC** | ESP32 `VIN` (5V) | Power (+5V, Red) | Powered from 5V rail |
| **GND** | ESP32 `GND` | Ground (Black) | Common ground |
| **TRIG** | ESP32 `GPIO 5` | Trigger Out (Green) | 10 µs pulse initiates acoustic ping |
| **ECHO** | ESP32 `GPIO 18` | Echo In (Green via divider) | Returns pulse width proportional to water depth |

---

### 2. GNSS Satellite Positioning (u-blox NEO-M8N GPS Module)
Provides real-time latitude, longitude, and ground speed for georeferencing bathymetric hazard points.
| NEO-M8N Pin | ESP32 Pin | Function / Wire Color | Notes |
|:---|:---|:---|:---|
| **VCC** | ESP32 `3V3` / `VIN` | Power (+3.3V/5V, Red) | On-board 3.3V LDO regulator |
| **GND** | ESP32 `GND` | Ground (Black) | Common ground |
| **TX** | ESP32 `GPIO 16` (RX2) | Serial Data (Green) | NMEA sentences @ 9600 / 115200 baud |
| **RX** | ESP32 `GPIO 17` (TX2) | Serial Config (Yellow) | UBX binary config commands |

---

### 3. Inertial Measurement Unit (MPU-6050 6-DOF IMU)
Measures vessel pitch and roll angles to dynamically correct sonar sounding distance when navigating water surface ripples:
$$\text{Depth}_{\text{corrected}} = \text{Depth}_{\text{measured}} \times \cos(\theta_{\text{pitch}}) \times \cos(\phi_{\text{roll}})$$
| MPU-6050 Pin | ESP32 Pin | Function / Wire Color | Notes |
|:---|:---|:---|:---|
| **VCC** | ESP32 `3V3` | Power (+3.3V, Red) | 3.3V logic rail |
| **GND** | ESP32 `GND` | Ground (Black) | Common ground |
| **SCL** | ESP32 `GPIO 22` | I2C Clock (Yellow) | Standard 400 kHz Fast I2C |
| **SDA** | ESP32 `GPIO 21` | I2C Data (Green) | Standard 400 kHz Fast I2C |

---

### 4. Dual Motor Driver (TB6612FNG Dual H-Bridge)
Drives two 12V DC thruster motors using differential steering (skid-steer mode).
| TB6612FNG Pin | ESP32 / Power Pin | Function / Wire Color | Notes |
|:---|:---|:---|:---|
| **VM** | 2S 18650 (+7.4V nominal) | Motor Power (+12V, Red) | Direct battery rail |
| **VCC** | ESP32 `3V3` | Logic Power (+3.3V, Red) | 3.3V logic level |
| **GND** | System `GND` | Ground (Black) | Star ground to battery & logic |
| **PWMA** | ESP32 `GPIO 25` | Left Motor Speed (Green) | Hardware PWM (LEDC channel 0) |
| **AIN1** | ESP32 `GPIO 26` | Left Motor Dir 1 (Green) | GPIO digital out |
| **AIN2** | ESP32 `GPIO 27` | Left Motor Dir 2 (Green) | GPIO digital out |
| **PWMB** | ESP32 `GPIO 14` | Right Motor Speed (Green)| Hardware PWM (LEDC channel 1) |
| **BIN1** | ESP32 `GPIO 12` | Right Motor Dir 1 (Green)| GPIO digital out |
| **BIN2** | ESP32 `GPIO 13` | Right Motor Dir 2 (Green)| GPIO digital out |
| **STBY** | ESP32 `GPIO 33` / `3V3`| Standby Enable (Yellow)  | High = Active, Low = Sleep |
| **AO1 / AO2** | Motor A (Left) | Left Thruster Terminals | 12V DC reversible |
| **BO1 / BO2** | Motor B (Right)| Right Thruster Terminals| 12V DC reversible |

---

### 5. Power Architecture
- **Battery**: 2S 18650 Li-ion pack (7.4V nominal, 8.4V peak, 2500mAh).
- **DC-DC Step-Down (Buck Converter)**: LM2596 / MP1584 switching regulator.
  - Input: 7.4V to 8.4V from 2S pack.
  - Output: Regulated 5.0V @ 2.5A to ESP32 `VIN` and sensor rails.
- **Star Grounding**: High-current motor ground and sensitive analog/RF grounds meet at a central terminal block to eliminate ground loop noise.
