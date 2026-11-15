# Test Plan

## 1. Purpose

This plan describes verification scenarios for the educational,
single-wheel ABS simulation. The scenarios map to unit tests and manual
console demonstrations in the implementation. They do not represent vehicle
validation, production ABS testing, or ISO 26262
certification evidence.

## 2. Test approach

- Test calculation and state logic independently where practical.
- Use deterministic inputs and expected results.
- Check boundary values on both sides of thresholds.
- Verify invalid and unavailable inputs do not produce a normal slip-based
  decision.
- Exercise timing-related faults using explicit cycle and response-time
  inputs, not wall-clock delays.
- Trace test cases to the software requirement IDs in
  [system_requirements.md](system_requirements.md).

## 3. Scenarios

| Test ID | Scenario | Setup / action | Expected result |
|---|---|---|---|
| TC-01 | Wheel linear speed | `wheelAngularSpeed = 10 rad/s`, `wheelRadius = 0.32 m`. | `Vwheel = 3.2 m/s` within the agreed numeric tolerance. |
| TC-02 | Nominal slip calculation | Valid inputs with `vehicleSpeed = 10 m/s`, `wheelAngularSpeed = 25 rad/s`, `wheelRadius = 0.32 m`. | `Vwheel = 8.0 m/s`; slip ratio is 0.20 (20%) within agreed tolerance. |
| TC-03 | Zero vehicle speed | Valid sensor status and `vehicleSpeed = 0`. | No division by zero; slip is unavailable and braking selects `NORMAL_BRAKING`. |
| TC-04 | Very low vehicle speed | Valid inputs at 0.5 m/s and just above. | Slip is unavailable at 0.5 m/s; calculation is valid above the cutoff. |
| TC-05 | Invalid numeric input | Try NaN, infinity, negative speed, non-positive radius, and brake-pedal values outside 0–100. | Input is rejected as F02; no normal slip-based decision uses it. |
| TC-06 | Sensor unavailable | Set `sensorStatus = false`. | F01 is reported; slip is not calculated from the unavailable sample; controller enters `FAULT`. |
| TC-07 | Slip below lower band | Valid braking scenario with slip below 15%, from reset. | State is `INCREASE` under the illustrative decision policy. |
| TC-08 | Slip in middle band | Valid braking scenario with slip between 15% and 20%, from reset. | State is `HOLD` under the illustrative decision policy. |
| TC-09 | Slip above upper band | Valid braking scenario with slip above 20%, from reset. | State is `DECREASE` under the illustrative decision policy. |
| TC-10 | Threshold boundaries | Exercise exactly 15% and exactly 20%, plus values immediately below and above from reset. | Initial selection follows `< 15%` -> `INCREASE`, `15–20%` inclusive -> `HOLD`, and `> 20%` -> `DECREASE`. |
| TC-11 | Hysteresis behavior | Apply samples around each switching boundary. | State transitions follow the 0.02 state-specific hysteresis rules in the system overview. |
| TC-12 | Standby and normal braking | Exercise 0% brake demand, positive braking below/equal to 0.5 m/s, and braking above the cutoff. | `STANDBY`, `NORMAL_BRAKING`, and slip-based states are selected as specified. |
| TC-13 | Stale sensor sample | Set controller cycle more than 2 cycles after the sample counter. | F03 is reported and the controller enters `FAULT`. |
| TC-14 | Watchdog timeout | Simulate response times at and above the 50 ms default. | 50 ms is accepted; a greater response reports F04 and enters `FAULT`. |
| TC-15 | Fault recovery / multiple faults | Follow a detected fault with healthy inputs; combine stale/unavailable input with watchdog timeout. | Healthy input can leave `FAULT`; sensor fault precedence is F01/F02/F03 and F04 can co-occur. |

## 4. Requirement traceability

| Requirements | Test coverage |
|---|---|
| SRS-01–SRS-05 | TC-05, TC-06 |
| SRS-06 | TC-01 |
| SRS-07–SRS-09 | TC-02–TC-04 |
| SRS-10–SRS-14 | TC-07–TC-12 |
| SRS-15 | TC-06 |
| SRS-16 | TC-05 |
| SRS-17 | TC-13 |
| SRS-18 | TC-14 |
| SRS-19–SRS-20 | TC-06, TC-13–TC-15 |
| SRS-21–SRS-22 | All unit tests and console demonstrations; modules are tested independently where applicable. |

## 5. Implemented test parameters

- Low-speed cutoff: 0.5 m/s, inclusive.
- Hysteresis: 0.02 slip-ratio value with state-specific transitions described
  in the system overview.
- Stale sample: more than 2 cycles behind the controller cycle.
- Watchdog: default 50 ms; timeout only when response time is greater than
  the threshold.
- Fault recovery: not latched; a healthy next controller update may leave
  `FAULT`.
- Numeric comparisons in unit tests use exact values where representable and
  a 1e-12 tolerance for floating-point calculations.

Console output is informational; automated tests assert module results
rather than parsing formatted text.

Thresholds in this plan are illustrative educational parameters, not official
ISO 26262 requirements or production calibration values.
