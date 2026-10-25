# Stage 1 Exploration: RealSense Depth Camera + Jetson Nano

## Overview
This directory contains the initial prototype software and trial data developed during the early exploration phase of the FloodScout project.

The initial concept was to mount an **Intel RealSense D455f stereo depth camera** paired with an **NVIDIA Jetson Nano** single-board computer on a roadside tripod or survey vehicle to detect waterlogged potholes and estimate water depth using computer vision and 3D point-cloud geometry.

---

## File: `pothole_depth.py`

### What it does:
1. **Camera Streaming**: Captures aligned 640x480 color (BGR) and 16-bit depth streams at 30 FPS using `pyrealsense2`.
2. **User ROI Selection**: Allows an operator to select a region of interest (ROI) enclosing the pothole using OpenCV (`cv2.selectROI`).
3. **Surrounding Road Surface Plane Fitting**:
   - Extracts a border ring (`PAD = 60 px`) around the selected bounding box.
   - Converts the depth pixels to 3D Cartesian coordinates $(x, y, z)$ in meters using camera intrinsic parameters (`ppx`, `ppy`, `fx`, `fy`).
   - Fits a reference ground plane to the dry road surface using Singular Value Decomposition (SVD) with 3 iterations of outlier rejection.
4. **Water Depth Estimation with Refraction Correction**:
   - Calculates the perpendicular drop distance from the pothole floor points to the fitted road plane:
     $$\text{drop} = -((P_{\text{roi}} - c) \cdot n)$$
   - Takes the 95th percentile depth drop and applies Snell's Law / optical refraction correction for water:
     $$\text{depth} = \max(\text{percentile}(\text{drop}, 95) \times 100 \times \mu, 0) \quad (\mu = 1.33)$$
   - Smooths readings using a running median filter (`deque(maxlen=7)`).
5. **Color-Coded Safety Classification**:
   - **GREEN (< 3.0 cm)**: Safe for light pedestrian & two-wheeler transit.
   - **AMBER (3.0 to 8.0 cm)**: Caution required; potential vehicle underbody scrape.
   - **RED (> 8.0 cm or No Depth Data)**: Severe hazard / trench; do not cross.

---

## Why This Approach Was Abandoned (Pivot to Stage 2 Acoustic Sonar)

During real-world field trials in stagnant and flooded street conditions, the optical/NIR approach encountered fundamental physical limitations:

1. **Turbidity & NIR Light Absorption**:
   - The Intel RealSense D455f uses active infrared structured light (850 nm IR projector) and stereo IR sensors.
   - Real urban floodwaters carry suspended mud, silt, sewage, and road grime (turbidity > 50 NTU).
   - Infrared light is rapidly absorbed and scattered within 2 to 5 cm of dirty water, resulting in total loss of depth return points (`roi < 50 points -> "NO DEPTH DATA"`).
2. **Surface Specular Reflections**:
   - Ripples, sky glint, and floating debris on the water surface caused false depth planes at the water surface rather than the submerged pothole bed.
3. **High Power Consumption & Weight**:
   - NVIDIA Jetson Nano + RealSense setup consumed **12W to 15W**, requiring bulky power banks and active fan cooling.
4. **Line-of-Sight & Stand-Off Range**:
   - Shallow camera grazing angles (< 30°) caused severe perspective occlusion of pothole edges.

### The Pivot:
Recognizing these optical barriers, the team pivoted to **Stage 2: FloodScout Catamaran Survey Boat**, replacing optical depth cameras with **submerged single-beam acoustic echosounders (Ping Sonar / JSN-SR04T transducer)** driven by an ultra-low-power **ESP32 microcontroller (2.2W total system draw)**. Sound waves penetrate turbid, muddy water with zero degradation, delivering millimeter-accurate bathymetry regardless of water clarity.

---

## Trial Photographs
See the `photos/` directory for photographic documentation of the laboratory bench and roadside test setups.
