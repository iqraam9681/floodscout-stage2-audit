# Bill of Materials (BOM) : FloodScout Survey Vessel

## Project Information
- **Project**: FloodScout : Autonomous Acoustic Bathymetric & Pothole Hazard Mapping USV
- **Application ID**: `VK26-IITH-SC-0543`
- **Competition**: Vishwakarma Awards 2026-27 (Stage 2)
- **Institution**: Mount Zion College of Engineering and Technology, Pudukkottai, Tamil Nadu
- **Faculty Mentor**: Asst. Prof. A. Sangeetha, Dept. of CSE

---

## Itemized Cost Breakdown

| Item # | Component | Part / Specification | Qty | Unit Cost (INR) | Total (INR) | Sourcing | Status |
|:---|:---|:---|:---:|:---:|:---:|:---|:---|
| 1 | Acoustic Echosounder | Blue Robotics Ping2 Sonar (115 kHz, 0.3-30 m range, UART) | 1 | ~₹36,000 | ~₹36,000 | Imported (USA) | Estimated, quote pending |
| 2 | MCU Mainboard | Espressif ESP32-S3 DevKitC-1 | 1 | ₹0 (Owned) | ₹0 | Local (India) | On hand |
| 3 | Alert Board MCU | ESP32 NodeMCU-32S | 1 | ₹500 | ₹500 | Domestic | Estimated, quote pending |
| 4 | GNSS Module | u-blox NEO-M8N + Ceramic Patch Antenna | 1 | ₹1,200 | ₹1,200 | Domestic | Estimated, quote pending |
| 5 | Inertial Sensor | MPU-6050 / BNO055 6-DOF IMU Breakout | 1 | ₹350 | ₹350 | Domestic | Estimated, quote pending |
| 6 | Motor Driver | Toshiba TB6612FNG Dual H-Bridge Driver | 1 | ₹650 | ₹650 | Domestic | Estimated, quote pending |
| 7 | Thrusters | Submersible 12V/7.4V DC Motors + 3-Blade Props | 2 | ₹1,200 | ₹2,400 | Domestic | Estimated, quote pending |
| 8 | Battery Cells | 18650 Li-ion 2500mAh Cells (2S configuration) | 2 | ₹0 (Owned) | ₹0 | Local (India) | On hand |
| 9 | Battery BMS | 2S 7.4V 10A Li-ion Protection BMS Board | 1 | ₹150 | ₹150 | Domestic | Estimated, quote pending |
| 10 | Voltage Regulation | Synchronous DC-DC Buck Converters (5V & 3.3V rails) | 2 | ₹110 | ₹220 | Domestic | Estimated, quote pending |
| 11 | Leak Detection | Conductive Grid Water/Leak Sensor | 1 | ₹120 | ₹120 | Domestic | Estimated, quote pending |
| 12 | Hull Materials | Twin PVC/XPS Closed-Cell Foam Floats + Acrylic Deck | 1 set | ₹1,600 | ₹1,600 | Domestic | Estimated, quote pending |
| 13 | Enclosure | IP67 Waterproof Box + PG7 Cable Glands + Gasket | 1 | ₹850 | ₹850 | Domestic | Estimated, quote pending |
| 14 | Alert Hardware | 3x LEDs (Green/Amber/Red) + Buzzer + Housing | 1 set | ₹450 | ₹450 | Domestic | Estimated, quote pending |
| 15 | Carrier PCB | Custom 2-Layer FR4 Prototype PCB (JLC/LionCircuits) | 1 | ₹800 | ₹800 | Domestic | Estimated, quote pending |
| **TOTAL** | | | | | **~₹45,290** | | |

---

## Notes on Component Selection
1. **Echosounder Choice**: The Blue Robotics Ping2 (115 kHz, 25-degree beam, 0.3-30 m range, open-source UART protocol) was selected over low-cost JSN-SR04T air-coupled transducers because it is specifically designed for submerged operation in direct water contact, providing accurate bathymetric soundings in turbid, dirty urban floodwater.
2. **Battery Architecture**: Existing owned 18650 Li-ion cells are configured in a 2S (7.4V) pack with a BMS protection board. A synchronous buck converter steps this down to the clean 5V and 3.3V logic rails, eliminating the need for a separate LiPo pack.
3. **Commercial Cost Comparison**: Professional single-beam survey USVs (e.g., SonarMite BTX, OceanAlpha SL20) cost upwards of ₹3,50,000 to ₹12,00,000. FloodScout delivers targeted flood-road hazard mapping capability at roughly 13% of the entry-level commercial alternative.
