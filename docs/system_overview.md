# System Overview

## 1. Project overview

ABS Simulation is an educational console application for an Embedded Systems
course project about automotive embedded software and ISO 26262 concepts. It
models the decisions of an anti-lock braking controller for one wheel
using simulated inputs. The project is intended to support discussion of
software architecture, requirements, state machines, fault handling, and
testing.

The simulation is not connected to a vehicle or hardware. It is not a
production ABS, a safety analysis, an ISO 26262-compliant product, or evidence
of certification. The project does not assign an ASIL or claim that its
illustrative thresholds are suitable for a real vehicle.

## 2. Objectives

- Build a modular and understandable C++17 software example.
- Simulate sensor values and validate them before use.
- Calculate wheel linear speed and wheel slip ratio safely.
- Demonstrate a one-wheel ABS state machine with hysteresis.
- Demonstrate fault detection and a simulated watchdog.
- Define test scenarios that can be traced to software requirements.
- Relate the development workflow to the V-Model and ISO 26262 Part 6 at an
  educational level, without claiming compliance.

## 3. Scope and limitations

### In scope

- One simulated wheel.
- Console input/output.
- Simulated vehicle speed, wheel angular speed, wheel radius, brake-pedal
  demand, and sensor availability.
- Slip calculation, state selection, fault reporting, watchdog behavior, and
  test cases.
- C++17 with GCC (MSYS2 UCRT64) as the intended toolchain.

### Out of scope

- Physical sensors, actuators, vehicles, or hardware-in-the-loop tests.
- Multi-wheel coordination, hydraulic pressure modeling, or a full vehicle
  dynamics model.
- Production calibration, real-world performance claims, or safety
  certification.
- Assigning an ASIL or asserting compliance with ISO 26262.

## 4. Conceptual architecture

The intended processing path is:

```text
Simulated inputs
      |
      v
Sensor Simulation ---> Fault Detection <--- Watchdog Simulation
      |                       |                    ^
      v                       v                    |
Input validation         Fault / FAULT state      |
      |                                            |
      v                                            |
Slip Ratio Calculation                            |
      |                                            |
      v                                            |
ABS State Machine ---------------------------------+
      |
      v
Console status and test results
```

The C++ implementation has corresponding header/source modules under
`include/` and `src/`. Tests exercise calculation and state logic
independently of console formatting.

## 5. Inputs and outputs

### Inputs

| Name | Type | Unit | Initial interpretation |
|---|---|---|---|
| `vehicleSpeed` | `double` | m/s | Non-negative simulated vehicle speed. |
| `wheelAngularSpeed` | `double` | rad/s | Non-negative simulated wheel angular speed. |
| `wheelRadius` | `double` | m | Positive radius; use 0.32 m as an example. |
| `brakePedal` | `double` | percent | Expected range 0–100. |
| `sensorStatus` | `bool` | none | True means available; false means unavailable. |

Each sample also has a `cycleCounter` used for deterministic freshness
checks. Controller cycle and simulated response duration are provided by the
scenario/test harness; neither uses wall-clock timing.

### Outputs

The console reports wheel linear speed when calculable, valid slip ratio,
ABS state, brake-pressure command, fault information, and watchdog status.
Unavailable calculations are displayed as `N/A`. No output actuates a real
brake.

## 6. System workflow

1. The simulation obtains or generates one set of input values.
2. Sensor availability and numeric values are validated.
3. Fault detection checks availability, values, and sample-cycle freshness.
4. If sensor data is valid and fresh, wheel linear speed and slip ratio are
   calculated (slip only above 0.5 m/s).
5. The watchdog evaluates the deterministic simulated response time.
6. The ABS state machine evaluates braking and slip; any reported fault
   selects `FAULT` and `NO_COMMAND`.
7. The simulation reports values, state, and faults to the console; tests
   check the outputs against defined expectations.

## 7. Slip ratio

Wheel linear speed:

```text
Vwheel = wheelAngularSpeed * wheelRadius
```

Slip ratio:

```text
SlipRatio = (vehicleSpeed - Vwheel) / vehicleSpeed
```

The second formula is used only when inputs are valid and vehicle speed is
greater than 0.5 m/s. At or below 0.5 m/s, wheel linear speed remains
available but slip is reported as unavailable. With positive brake demand
the controller selects `NORMAL_BRAKING`; no-brake demand selects `STANDBY`.

For interpretation, a ratio of 0.15 corresponds to 15%. Negative finite slip
is preserved if wheel peripheral speed exceeds vehicle speed; slip is not
clamped.

## 8. ABS states

| State | Intended educational meaning |
|---|---|
| `STANDBY` | Brake pedal is exactly 0%; command is `NO_COMMAND`. |
| `NORMAL_BRAKING` | Positive brake demand at or below 0.5 m/s; command is `NO_COMMAND`, because slip is unavailable. |
| `INCREASE` | Illustrative slip below 15%; conceptual brake application may increase. |
| `HOLD` | Illustrative slip from 15% through 20%; conceptual brake application may be held. |
| `DECREASE` | Illustrative slip above 20%; conceptual brake application may decrease. |
| `FAULT` | A fault prevents normal decision-making, or a fault response is active. |

The labels describe simulated decisions only, not physical pressure commands.
The 15% and 20% values and 0.02 hysteresis are educational examples. Initial
entry from `STANDBY` or `NORMAL_BRAKING` uses the direct bands: below 0.15
selects `INCREASE`, up to and including 0.20 selects `HOLD`, and above 0.20
selects `DECREASE`. Hysteresis applies on subsequent transitions:

- From `INCREASE`, remain there below 0.17; above 0.20 jump to `DECREASE`;
  otherwise select `HOLD`.
- From `HOLD`, select `INCREASE` below 0.13 or `DECREASE` above 0.22;
  otherwise remain in `HOLD`.
- From `DECREASE`, remain there above 0.18; below 0.15 jump to `INCREASE`;
  otherwise select `HOLD`.

The command maps directly from the three modulation states to
`PRESSURE_INCREASE`, `PRESSURE_HOLD`, and `PRESSURE_DECREASE`. The other
states and `FAULT` produce `NO_COMMAND`. A fault is not latched: on a healthy
cycle the controller evaluates the current input and can return directly to
the appropriate operating state.

## 9. Fault behavior

- **F01 – Sensor unavailable:** indicated by `sensorStatus == false`.
- **F02 – Invalid sensor data:** non-finite values, invalid ranges, an
  overflowing wheel-speed product, or a sample counter ahead of the
  controller cycle.
- **F03 – Sensor data not updated:** controller cycle is more than 2 cycles
  ahead of the sample counter.
- **F04 – Controller execution timeout detected by watchdog:** simulated
  response exceeds the configurable 50 ms default threshold.

Sensor fault priority is F01, F02, then F03; only the first matching sensor
fault is reported. F04 is independently added when the watchdog times out,
so it can accompany a sensor fault. The controller enters `FAULT` whenever
any fault is present. This simulation's fault response is not a validated
safe state for a real ABS.

The watchdog defaults to 50 ms and detects a timeout only when the
deterministically supplied response time is greater than that value. The
watchdog tracks controller cycles and can be reset. It does not measure real
execution time and is not hardware watchdog functionality.

## 10. Development and verification approach

Requirements are mapped to architecture, unit-level tests, and system
demonstration scenarios. This offers an educational illustration of V-Model
traceability. It does not
establish that the project follows all ISO 26262 lifecycle, process, or
verification obligations.

See [system_requirements.md](system_requirements.md) for the requirement set
and [test_plan.md](test_plan.md) for test scenarios and traceability.
