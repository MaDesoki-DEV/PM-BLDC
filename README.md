# PM-BLDC — Permanent Magnet Brushless DC Motor Mathematical Model

A MATLAB/Simulink project developed as part of Master's studies, implementing a mathematical model of a Permanent Magnet Brushless DC (PM-BLDC) motor.

## Overview

Brushless DC (BLDC) motors — also referred to as Permanent Magnet Synchronous Motors (PMSM) in some contexts — are widely used in industrial drives, electric vehicles, robotics, and aerospace applications due to their high efficiency, high power density, and low maintenance requirements. This project builds a complete mathematical model of a PM-BLDC motor in the MATLAB/Simulink environment, enabling simulation and analysis of motor behaviour under various operating conditions.

## Repository Structure

```
PM-BLDC/
├── Mathimatical Model/
│   └── PM_BLDC_Mathematical_Model.slx   # Simulink model of the PM-BLDC motor
├── resources/
│   └── project/                         # MATLAB project metadata
├── PMBLDC.prj                           # MATLAB project file
└── README.md
```

## Requirements

| Tool | Version |
|------|---------|
| MATLAB | R2021a or later (recommended) |
| Simulink | Included with MATLAB |

No additional MATLAB toolboxes are required beyond the base Simulink installation.

## Getting Started

1. **Open the project** — In MATLAB, open `PMBLDC.prj` to load the project and set up the correct paths automatically.
2. **Define workspace parameters** — Before running the simulation, define the required motor and simulation parameters in the MATLAB workspace (see [Workspace Parameters](#workspace-parameters) below).
3. **Run the model** — Open `Mathimatical Model/PM_BLDC_Mathematical_Model.slx` in Simulink and click **Run**.

## Workspace Parameters

The Simulink model relies on parameters that must be defined in the MATLAB base workspace before starting a simulation. Open the model for the full list of annotated parameters; typical parameters include:

| Parameter | Description | Units |
|-----------|-------------|-------|
| `R` | Stator phase resistance | Ω |
| `L` | Stator phase inductance | H |
| `Ke` | Back-EMF constant | V·s/rad |
| `Kt` | Torque constant | N·m/A |
| `J` | Rotor moment of inertia | kg·m² |
| `B` | Viscous friction coefficient | N·m·s/rad |
| `P` | Number of pole pairs | — |

> **Note:** All required parameters are annotated directly inside the Simulink model. Refer to the model annotations for the exact names and recommended values.

## Model Description

The mathematical model captures the key dynamics of a PM-BLDC motor:

- **Electrical subsystem** — Voltage equations for each stator phase, including resistance, inductance, and trapezoidal back-EMF.
- **Electromechanical coupling** — Conversion of phase currents and back-EMF to electromagnetic torque.
- **Mechanical subsystem** — Newton's second law for rotational motion, accounting for load torque, friction, and inertia.

## License

This project was created for academic purposes. Please contact the author before reuse.
