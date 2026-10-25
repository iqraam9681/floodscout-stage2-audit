# CAD Models & Structural Assembly (FloodScout)

## Overview
This directory is designated for the mechanical CAD models, 3D printing STL meshes, and structural analysis files of the **FloodScout Autonomous Catamaran Survey Vessel**.

---

## Folder Structure (Drop Files Here)
```
cad/
|-- assembly/        <- Complete vessel assemblies (.STEP, .SLDASM, .F3D)
|-- hulls/           <- Catamaran left & right float hulls (.STEP, .STL)
|-- electronics_bay/ <- IP67 sealed avionics mounting tray & lid (.STEP, .STL)
|-- motor_mounts/    <- Dual thruster clamp brackets & rudder struts (.STEP, .STL)
\-- drawings/        <- Dimensioned 2D engineering drawings & cut sheets (.PDF, .DXF)
```

---

## Hull Design Specifications
- **Vessel Architecture**: Symmetrical Catamaran (Twin-Hull)
- **Overall Dimensions**: 650 mm (L) × 420 mm (W) × 180 mm (H)
- **Draft at Full Payload (2.8 kg)**: 45 mm (enables navigation in water depths as shallow as 8 to 10 cm)
- **Displacement**: 5.4 Liters total buoyancy (~2.6 kg reserve buoyancy)
- **Hull Material**: Rotomolded High-Density Polyethylene (HDPE) or 3D-printed PETG (3 perimeters, 25% gyroid infill, epoxy coated).
- **Sensor Keel Mount**: Central through-hull wet bay for acoustic echosounder probe with acoustic isolation rubber grommet.

---

> **Note for Hardware Team**:
> Upload exported STEP (.step / .stp) and SolidWorks (.SLDPRT / .SLDASM) models to their respective folders above.
