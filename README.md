# ABS Simulation

An educational, console-based C++17 project for simulating the ABS decision
logic of one wheel. The project is intended for an Embedded Systems course
and to support discussion of software architecture, requirements, testing,
and ISO 26262 concepts.

## Scope and limitations

- One simulated wheel; no physical hardware or vehicle connection.
- GCC (MSYS2 UCRT64) and C++17 are the intended development configuration.
- Slip thresholds, hysteresis, cutoff, and watchdog timings are educational
  simulation values, not validated production calibration.
- This project is not ISO 26262 compliant or certified. It assigns no ASIL
  and makes no safety claim.

## Documentation

- [System requirements](docs/system_requirements.md) — software requirements,
  input/output rules, and fault catalog.
- [System overview](docs/system_overview.md) — architecture, workflow,
  formula, state transitions, and implementation decisions.
- [Test plan](docs/test_plan.md) — test scenarios, expected outcomes, and
  requirement traceability.

## Architecture and module responsibilities

- `Sensor` (`include/Sensor.h`, `src/Sensor.cpp`) stores configurable sample
  inputs and validates finite values and allowed ranges.
- `SlipRatioCalculator` (`include/SlipRatio.h`, `src/SlipRatio.cpp`) computes
  wheel linear speed and slip, and reports valid, low-speed, unavailable, or
  invalid status.
- `ABSController` (`include/ABSController.h`, `src/ABSController.cpp`)
  implements the state machine, hysteresis, and educational pressure
  commands.
- `FaultDetection` (`include/FaultDetection.h`, `src/FaultDetection.cpp`)
  detects sensor availability, invalid data, and stale cycle counters.
- `Watchdog` (`include/Watchdog.h`, `src/Watchdog.cpp`) deterministically
  compares simulated response time with a configurable timeout.
- `src/main.cpp` provides the scenario menu and formatted result display.
- `tests/` contains a standalone test runner with no external test framework.

## Build instructions

### CMake (MSYS2 UCRT64)

Open the MSYS2 UCRT64 terminal, ensure GCC and CMake are on `PATH`, then run
from the project root:

```sh
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

### Direct GCC build

Run from the project root in the MSYS2 UCRT64 terminal:

```sh
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp src/Sensor.cpp src/SlipRatio.cpp src/ABSController.cpp src/FaultDetection.cpp src/Watchdog.cpp -o abs_simulation.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude -Itests tests/test_main.cpp tests/test_sensor.cpp tests/test_slip_ratio.cpp tests/test_abs_controller.cpp tests/test_fault_detection.cpp tests/test_watchdog.cpp src/Sensor.cpp src/SlipRatio.cpp src/ABSController.cpp src/FaultDetection.cpp src/Watchdog.cpp -o abs_tests.exe
```

## Run instructions

After a CMake build, run:

```sh
./build/abs_simulation.exe
```

For the direct GCC build, run `./abs_simulation.exe`. Select a numbered
scenario from the menu; option 8 runs all seven demonstration scenarios.

## Test instructions

With CMake:

```sh
ctest --test-dir build --output-on-failure
./build/abs_tests.exe
```

Or, after the direct GCC build, run `./abs_tests.exe`. The standalone test
runner prints PASS or FAIL for every test and returns a failing exit code if
any test fails.

## Simulation scenarios

1. Normal braking (10% illustrative slip).
2. Moderate braking (18% illustrative slip).
3. Emergency braking (26% illustrative slip).
4. Sensor unavailable (F01).
5. Invalid wheel speed input (F02).
6. Stale sensor sample (F03).
7. Simulated controller response over the watchdog limit (F04).

## Implemented educational decisions

- Wheel radius example: 0.32 m.
- Slip is not calculated at vehicle speed less than or equal to 0.5 m/s.
- Braking demand is any valid `brakePedal > 0`; no braking is exactly 0%.
- At or below the low-speed cutoff with braking requested, the state is
  `NORMAL_BRAKING`; otherwise slip bands select `INCREASE`, `HOLD`, or
  `DECREASE`.
- Slip bands use 0.15 and 0.20 ratios. Hysteresis is 0.02, with state-specific
  entry and exit rules detailed in the system overview.
- Sensor data is stale when the controller cycle exceeds its sample counter
  by more than 2 cycles. Sensor fault checks report one highest-priority
  sensor fault: unavailable, invalid, then stale.
- The default watchdog threshold is 50 ms; a simulated response times out
  only when it is greater than the threshold. No wall-clock timing or random
  delay is used.
- Fault state is not latched: the controller evaluates and may leave it on the
  next healthy cycle. `reset()` also returns the controller to `STANDBY`.

## Known limitations and safety note

This is a deterministic teaching demonstration, not a physical ABS model.
It does not model vehicle dynamics, actuator pressure, multiple wheels,
real-time scheduling, actual sensors, or hardware watchdog behavior. Its
thresholds and responses are not production calibration. The project is
not an ISO 26262-compliant automotive system and makes no certification,
ASIL, or real-world safety claim.
