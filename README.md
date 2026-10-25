# FloodScout: Autonomous Acoustic Bathymetric & Pothole Hazard Mapping USV

[![Vishwakarma Awards 2026-27](https://img.shields.io/badge/Competition-Vishwakarma%20Awards%202026--27-orange.svg)](docs/COMPETITION_SUBMISSION.md)
[![Application ID](https://img.shields.io/badge/Application%20ID-VK26--IITH--SC--0543-blue.svg)](docs/COMPETITION_SUBMISSION.md)
[![Institution](https://img.shields.io/badge/Institution-Mount%20Zion%20CET%2C%20Pudukkottai-navy.svg)](http://www.mountzion.ac.in)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

> **Stand-off acoustic sounding survey vessel for detecting and mapping submerged potholes, craters, and open utility trenches in inundated urban road corridors.**

---

## Problem Statement & Context
During monsoon flash floods in Indian urban centers, standing turbid water (10 cm to 1.5 m deep) conceals catastrophic road hazards : deep potholes, washed-away culverts, and open manhole pits. 

Pedestrians, two-wheelers, and emergency response vehicles routinely wade blindly into inundated roads, resulting in severe vehicle drownings, fatal injuries, and transit paralysis. Conventional bathymetric survey boats cost upwards of ₹3,50,000 ($4,200), making them entirely inaccessible for municipal transit departments and local emergency first responders.

**FloodScout** is a lightweight, low-draft catamaran Unmanned Surface Vessel (USV) costing under **₹15,000 ($178)** that allows an operator to stand safely on a dry curb or footpath and deploy an acoustic sounding boat that maps hidden roadbed depth in real time at 10 Hz.

---

## System Architecture & Wiring Diagram

The vessel combines single-beam acoustic sounding with IMU attitude tilt compensation and GNSS spatial georeferencing:

![FloodScout System Wiring Diagram](schematics/circuit_diagram_components.jpg)

### Subsystem Interconnect Matrix:
```
               +-------------------------------------------+
               |     3S LiPo Battery (11.1V / 2200mAh)     |
               +---------------------+---------------------+
                                     |
                                     v
                        [ LM2596 Buck Converter ]
                                     | (Regulated 5.0V / 3A)
                                     v
+------------------------+    +--------------+    +-----------------------+
|  JSN-SR04T / Ping2     |--->|   ESP32-S3   |<---|  u-blox NEO-M8N GPS   |
|  Acoustic Depth Sensor |    | Dual-Core MCU|    |  Georeferenced Points |
+------------------------+    +-------+------+    +-----------------------+
                                      |
         +----------------------------+----------------------------+
         | (I2C 400kHz)                                            | (PWM LEDC 20kHz)
         v                                                         v
+------------------------+                                +-----------------------+
|   MPU-6050 6-DOF IMU   |                                |  TB6612FNG Dual Driver|
|  Pitch & Roll Tilt     |                                |  Reversible H-Bridge  |
+------------------------+                                +-----------+-----------+
                                                                      |
                                                       +--------------+--------------+
                                                       |                             |
                                                       v                             v
                                            [ Left DC Thruster ]          [ Right DC Thruster ]
```

---

## The Architectural Pivot: Stage 1 vs. Stage 2

A core milestone of this project was the **rigorous real-world failure analysis** of optical depth cameras during Stage 1 exploration, leading to our acoustic pivot:

| Feature / Metric | Stage 1: Optical Exploration (Failed) | Stage 2: FloodScout Acoustic USV (Adopted) |
|:---|:---|:---|
| **Primary Sensor** | Intel RealSense D455f Stereo Depth Camera | Single-Beam Waterproof Acoustic Echosounder |
| **Compute Engine** | NVIDIA Jetson Nano (15W draw) | ESP32-S3 Dual-Core MCU (2.2W total system draw) |
| **Water Penetration** | **Failed in muddy floodwater** (850nm NIR absorbed/scattered within 2 to 5 cm) | **100% Penetration** (Acoustic pressure waves ignore water turbidity & silt) |
| **Power & Weight** | Bulky 12V 10Ah battery pack, 1.8 kg payload | Compact 3S LiPo (2200mAh), 4.5 hours continuous endurance |
| **Total Hardware Cost**| > ₹45,000 ($540 USD) | **₹14,800 ($178 USD)** |
| **Deployment Mode** | Tripod / vehicle rooftop (poor grazing angle) | Low-draft catamaran vessel operating directly on water surface |

*For complete python scripts, OpenCV plane-fitting algorithms, and bench trial photos from Stage 1, see the [`stage1-exploration/`](stage1-exploration/) folder.*

---

## Repository Structure

```
|-- README.md               <- Project master overview & system documentation
|-- LICENSE                 <- MIT Open-Source Hardware/Software License
|-- cad/                    <- Catamaran hull, electronics bay, and motor mount CAD files
|   \-- README.md           <- Mechanical specs & file upload instructions
|-- firmware/               <- Embedded C++ firmware for ESP32
|   |-- floodscout_esp32_firmware.ino <- Core bathymetric sounding & navigation code
|   \-- README.md           <- Compilation and flashing instructions
|-- schematics/             <- Visual circuit diagrams & wiring pinouts
|   |-- circuit_diagram_components.jpg <- High-resolution component wiring diagram
|   \-- WIRING.md           <- Complete pin-by-pin interconnection matrix
|-- bom/                    <- Itemized Bill of Materials
|   |-- BOM.md              <- Hardware cost breakdown (₹14,800 / $178 USD)
|   \-- BOM.csv             <- Spreadsheet-compatible CSV export
|-- stage1-exploration/     <- RealSense D455f + Jetson Nano exploration archives
|   |-- pothole_depth.py    <- Original Python depth-plane algorithm script
|   |-- README.md           <- Failure analysis and acoustic pivot rationale
|   \-- photos/             <- Laboratory and field trial setup photos
\-- docs/                   <- Competition compliance and dossiers
    \-- COMPETITION_SUBMISSION.md <- Vishwakarma Awards 2026-27 Stage 2 audit dossier
```

---

## Bill of Materials Summary (₹14,800 Total)

| Component | Subsystem | Quantity | Total (INR) | Total (USD) |
|:---|:---|:---:|:---:|:---:|
| ESP32-S3 Dual-Core Microcontroller | Main Compute | 1 | ₹650 | $7.80 |
| Waterproof Acoustic Echosounder (JSN-SR04T / Ping2) | Bathymetry | 1 | ₹1,200 | $14.45 |
| u-blox NEO-M8N Multi-GNSS + Ceramic Antenna | Navigation | 1 | ₹1,450 | $17.45 |
| InvenSense MPU-6050 6-DOF IMU | Dynamic Tilt Correction | 1 | ₹280 | $3.35 |
| Toshiba TB6612FNG Dual H-Bridge Driver | Actuation | 1 | ₹350 | $4.20 |
| 12V High-Torque Waterproof Thruster Motors | Marine Propulsion | 2 | ₹3,600 | $43.35 |
| 3S LiPo Battery Pack (11.1V, 2200mAh, 25C) | Power Storage | 1 | ₹1,850 | $22.30 |
| LM2596 DC-DC Step-Down Buck Converter | Voltage Regulation | 1 | ₹220 | $2.65 |
| Catamaran Twin Hulls (Rotomolded HDPE / 3D PETG) | Chassis | 2 | ₹3,800 | $45.75 |
| IP67 Waterproof Gasketed Electronics Bay | Ingress Enclosure | 1 | ₹850 | $10.25 |
| ESP-NOW 2.4 GHz Long-Range RF Link | Wireless Telemetry | 1 | ₹550 | $6.65 |
| **TOTAL** | | | **₹14,800** | **$178.20** |

*Full component specifications and supplier links are documented in [`bom/BOM.md`](bom/BOM.md).*

---

## Student Innovation Team
- **Iqraam M** (Lead Embedded Hardware & Bathymetry)
- **Hari Ramachandran V** (Hydrodynamic Catamaran Hull & Structures)
- **Ram Santhosh R** (Telemetry, Algorithms & GIS Hazard Mapping)

### Faculty Mentorship
- **Asst. Prof. A. Sangeetha**, M.Tech., Ph.D.*
  - Assistant Professor, Department of Computer Science & Engineering
  - Mount Zion College of Engineering and Technology (MZCET), Pudukkottai, Tamil Nadu
