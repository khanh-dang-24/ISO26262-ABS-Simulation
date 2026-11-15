# System Requirements

## 1. Purpose

This document defines the software requirements for the educational,
single-wheel Anti-lock Braking System (ABS) simulation. Requirements are
identified by unique IDs so that they can be traced to architecture and tests
as the project develops.

The requirements describe a console-based simulation written in C++17. They
are not official ISO 26262 requirements, production ABS calibration values, or
a claim of compliance with any automotive safety standard.

## 2. Scope and limitations

The program models one wheel and controller decisions that would influence
braking. It does not connect to a real vehicle, control a physical brake, or
demonstrate that any real-world braking hazard is mitigated. Sensor data and
controller timing are simulated.

The implemented modules are:

1. Sensor Simulation
2. Slip Ratio Calculation
3. ABS State Machine
4. Fault Detection
5. Watchdog Simulation
6. Test Cases

## 3. Input specification

| Input | Type | Unit / range | Description |
|---|---|---|---|
| `vehicleSpeed` | `double` | m/s; expected non-negative | Simulated vehicle speed. |
| `wheelAngularSpeed` | `double` | rad/s; expected non-negative | Simulated wheel angular speed. |
| `wheelRadius` | `double` | m; must be positive; example: 0.32 | Effective wheel radius used in the linear-speed calculation. |
| `brakePedal` | `double` | percent, expected 0–100 | Simulated brake-pedal demand. |
| `sensorStatus` | `bool` | `true` / `false` | Indicates whether the simulated sensor is available. |

Inputs are supplied by fixed demonstration scenarios and the test harness.
Each sensor sample has a monotonically increasing cycle counter for stale-data
detection.

## 4. Output specification

The simulation is expected to expose, at minimum:

- Calculated wheel linear speed, in m/s.
- Calculated slip ratio when the calculation is valid.
- Current ABS state.
- Detected fault code(s), if any.

The application displays these values to the console. It does not produce a
physical actuator command. Pressure commands are labels for educational
state-machine outputs only; they do not actuate hardware.

## 5. Functional software requirements

### Input and sensor handling

- **SRS-01:** The system shall accept `vehicleSpeed`, `wheelAngularSpeed`,
  `wheelRadius`, `brakePedal`, and `sensorStatus` as simulation inputs.
- **SRS-02:** The system shall use 0.32 m as the documented example wheel
  radius while allowing the radius to be supplied as an input.
- **SRS-03:** The system shall validate sensor availability and numeric sensor
  values before calculating slip ratio.
- **SRS-04:** The system shall reject non-finite numeric values, negative
  vehicle or wheel speed, non-positive wheel radius, brake-pedal values
  outside the inclusive 0–100 percent range, non-finite derived wheel speed,
  and sensor sample counters ahead of the controller cycle.
- **SRS-05:** When sensor data is unavailable or invalid, the system shall not
  use that data to calculate slip ratio and shall report the applicable fault.

### Slip calculation

- **SRS-06:** The system shall calculate wheel linear speed as
  `Vwheel = wheelAngularSpeed * wheelRadius`.
- **SRS-07:** For a valid input with vehicle speed above 0.5 m/s, the system
  shall calculate slip ratio as
  `SlipRatio = (vehicleSpeed - Vwheel) / vehicleSpeed`.
- **SRS-08:** The system shall treat vehicle speeds at or below 0.5 m/s as
  low-speed input, avoid division by zero, and report slip as unavailable.
- **SRS-09:** The displayed slip ratio shall be identified as a ratio (or
  percentage if converted for display); internal units and display rounding
  shall be consistent.

### ABS state machine

- **SRS-10:** The system shall represent the states `STANDBY`,
  `NORMAL_BRAKING`, `INCREASE`, `HOLD`, `DECREASE`, and `FAULT`.
- **SRS-11:** With valid braking data, the illustrative slip decision bands
  shall be: below 15% -> `INCREASE`, from 15% through 20% -> `HOLD`, and
  above 20% -> `DECREASE`.
- **SRS-12:** The state machine shall use 0.02 slip-ratio hysteresis with
  state-specific entry and exit rules documented in the system overview.
- **SRS-13:** Exactly 0% brake demand shall select `STANDBY`; positive brake
  demand at or below the low-speed cutoff shall select `NORMAL_BRAKING`.
  Above the cutoff, the slip bands select a modulation state.
- **SRS-14:** When a detected fault is present, the state machine shall enter
  `FAULT` and shall not issue a normal pressure command for that cycle.

### Fault detection and watchdog

- **SRS-15:** The system shall detect an unavailable sensor and report fault
  **F01: Sensor unavailable**.
- **SRS-16:** The system shall detect invalid sensor values and report fault
  **F02: Invalid sensor data**.
- **SRS-17:** The system shall report **F03: Sensor data not updated** when
  the controller cycle exceeds the sensor sample cycle by more than 2 cycles.
- **SRS-18:** The system shall simulate a watchdog with a configurable
  threshold (default 50 ms) that detects controller execution exceeding the
  limit and report fault
  **F04: Controller execution timeout detected by watchdog**.
- **SRS-19:** A detected fault shall be visible in the simulation output and
  associated with the `FAULT` state where applicable.
- **SRS-20:** Sensor fault detection shall report the first applicable fault
  in this order: F01 unavailable, F02 invalid data, F03 stale data. F04 may
  additionally be reported for a cycle. Faults shall not latch; a healthy
  following cycle may leave the `FAULT` state.

### Testability

- **SRS-21:** The software design shall keep sensor simulation, slip
  calculation, state-machine logic, fault detection, watchdog simulation, and
  tests sufficiently modular for independent testing.
- **SRS-22:** Test cases shall cover nominal behavior, threshold boundaries,
  low-speed handling, invalid or unavailable sensors, stale data, and
  watchdog timeout behavior.

## 6. Fault catalog

| ID | Fault | Detection rule | Handling |
|---|---|---|---|
| F01 | Sensor unavailable | `sensorStatus` is false. | Highest-priority sensor fault; healthy next cycle clears the active fault. |
| F02 | Invalid sensor data | Non-finite values, negative speed, non-positive radius, invalid pedal range, non-finite derived wheel speed, or a future sample cycle. | Reports before stale-data check. |
| F03 | Sensor data not updated | Current controller cycle minus sample cycle is greater than 2. | Cycle counter models freshness; it is not wall-clock sampling. |
| F04 | Controller execution timeout detected by watchdog | Simulated response time is greater than the configurable threshold (default 50 ms). | Deterministic input; equal-to-threshold is not a timeout. |

## 7. Educational parameter notice

The 15% and 20% slip thresholds, 0.02 hysteresis, 0.5 m/s cutoff, 0.32 m
example radius, and watchdog values are illustrative simulation parameters.
They are not official ISO 26262 requirements, validated vehicle data, or
production ABS calibration values.
