# PM-BLDC Mathematical Model

This folder contains the Simulink model (`PM_BLDC_Mathematical_Model.slx`) that implements the mathematical model of a Permanent Magnet Brushless DC (PM-BLDC) motor.

## File

| File | Description |
|------|-------------|
| `PM_BLDC_Mathematical_Model.slx` | Simulink model of the PM-BLDC motor |

## How to Run

1. Open MATLAB and load the project by opening `PMBLDC.prj` in the repository root.
2. Define all required workspace parameters in the MATLAB base workspace (see the root [README](../README.md#workspace-parameters) and the annotations inside the model).
3. Open `PM_BLDC_Mathematical_Model.slx` and click **Run**.

## Model Structure

The model is divided into three main subsystems:

### 1. Electrical Subsystem
Models the stator voltage equations for each of the three phases:

$$V_{ph} = R \cdot i_{ph} + L \cdot \frac{di_{ph}}{dt} + e_{ph}$$

where $V_{ph}$ is the phase voltage, $R$ is the stator resistance, $L$ is the stator inductance, $i_{ph}$ is the phase current, and $e_{ph}$ is the back-EMF of that phase.

### 2. Back-EMF Generation
Generates the trapezoidal back-EMF waveforms as a function of rotor position $\theta$:

$$e_{ph} = K_e \cdot \omega \cdot f(\theta)$$

where $K_e$ is the back-EMF constant, $\omega$ is the rotor angular speed, and $f(\theta)$ is the trapezoidal function of rotor position.

### 3. Mechanical Subsystem
Models the rotor dynamics using Newton's second law for rotational motion:

$$J \cdot \frac{d\omega}{dt} = T_e - T_L - B \cdot \omega$$

where $J$ is the moment of inertia, $T_e$ is the electromagnetic torque, $T_L$ is the load torque, and $B$ is the viscous friction coefficient.

## Required Workspace Parameters

All parameters below must be defined in the MATLAB base workspace before running the simulation. Annotations inside the model indicate where each parameter is used.

| Parameter | Description | Units |
|-----------|-------------|-------|
| `R` | Stator phase resistance | Ω |
| `L` | Stator phase inductance | H |
| `Ke` | Back-EMF constant | V·s/rad |
| `Kt` | Torque constant | N·m/A |
| `J` | Rotor moment of inertia | kg·m² |
| `B` | Viscous friction coefficient | N·m·s/rad |
| `P` | Number of pole pairs | — |
