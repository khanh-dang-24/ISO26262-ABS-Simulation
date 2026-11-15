#include "ABSController.h"
#include "FaultDetection.h"
#include "Sensor.h"
#include "SlipRatio.h"
#include "Watchdog.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace
{
struct Scenario
{
    std::string name;
    SensorData sensorData;
    std::uint64_t currentCycle;
    std::uint64_t responseTimeMs;
};

Scenario makeScenario(const std::string& name,
                      double vehicleSpeed,
                      double wheelAngularSpeed,
                      double brakePedal,
                      std::uint64_t sampleCycle = 1,
                      bool sensorStatus = true)
{
    SensorData data;
    data.vehicleSpeed = vehicleSpeed;
    data.wheelAngularSpeed = wheelAngularSpeed;
    data.wheelRadius = 0.32;
    data.brakePedal = brakePedal;
    data.sensorStatus = sensorStatus;
    data.cycleCounter = sampleCycle;
    return {name, data, sampleCycle, 10};
}

std::vector<Scenario> allScenarios()
{
    Scenario normal = makeScenario("Normal braking", 20.0, 56.25, 30.0);
    Scenario moderate = makeScenario("Moderate braking", 20.0, 51.25, 55.0);
    Scenario emergency = makeScenario("Emergency braking", 20.0, 46.25, 90.0);
    Scenario sensorFailure =
        makeScenario("Sensor failure", 20.0, 56.25, 30.0, 1, false);

    Scenario invalid = makeScenario("Invalid sensor data", 20.0, -1.0, 30.0);

    Scenario sensorTimeout = makeScenario("Sensor timeout", 20.0, 56.25, 30.0, 1);
    sensorTimeout.currentCycle = 5;

    Scenario watchdogTimeout =
        makeScenario("Watchdog timeout", 20.0, 56.25, 30.0);
    watchdogTimeout.responseTimeMs = 75;

    return {normal, moderate, emergency, sensorFailure,
            invalid, sensorTimeout, watchdogTimeout};
}

void runScenario(const Scenario& scenario)
{
    constexpr std::uint64_t watchdogTimeoutMs = 50;
    Sensor sensor(scenario.sensorData);
    FaultDetection faultDetection(2);
    SlipRatioCalculator slipCalculator;
    ABSController controller;
    Watchdog watchdog(watchdogTimeoutMs);

    const SensorData& data = sensor.read();
    std::vector<Fault> faults =
        faultDetection.detect(data, scenario.currentCycle);
    SlipCalculationResult slip;
    if (faults.empty())
    {
        slip = slipCalculator.calculate(data);
    }
    else
    {
        slip.message = "Calculation skipped because a sensor fault was detected.";
    }

    const WatchdogStatus watchdogStatus =
        watchdog.monitorCycle(scenario.responseTimeMs);
    if (watchdogStatus.timedOut)
    {
        faults.push_back({FaultCode::F04_CONTROLLER_TIMEOUT,
                          FaultDetection::messageFor(
                              FaultCode::F04_CONTROLLER_TIMEOUT)});
    }

    const ABSControllerOutput output =
        controller.update(data.brakePedal, slip, !faults.empty());

    std::cout << "\n=== " << scenario.name << " ===\n"
              << std::fixed << std::setprecision(2)
              << "Vehicle speed:        " << data.vehicleSpeed << " m/s\n"
              << "Wheel angular speed:  " << data.wheelAngularSpeed << " rad/s\n"
              << "Wheel radius:         " << data.wheelRadius << " m\n"
              << "Wheel linear speed:   ";
    if (slip.hasWheelLinearSpeed())
    {
        std::cout << slip.wheelLinearSpeed << " m/s\n";
    }
    else
    {
        std::cout << "N/A\n";
    }

    std::cout << "Brake pedal:          " << data.brakePedal << " %\n"
              << "Slip ratio:           ";
    if (slip.hasValidSlip())
    {
        std::cout << std::setprecision(4) << slip.slipRatio
                  << " (" << std::setprecision(2)
                  << slip.slipRatio * 100.0 << " %)\n";
    }
    else
    {
        std::cout << "N/A (" << slip.message << ")\n";
    }

    std::cout << "ABS state:            "
              << ABSController::stateName(output.state) << '\n'
              << "Pressure command:     "
              << ABSController::commandName(output.pressureCommand) << '\n'
              << "Fault status:         ";
    if (faults.empty())
    {
        std::cout << "No faults\n";
    }
    else
    {
        std::cout << '\n';
        for (const Fault& fault : faults)
        {
            std::cout << "  " << FaultDetection::codeName(fault.code)
                      << ": " << fault.message << '\n';
        }
    }

    std::cout << "Watchdog status:      " << watchdogStatus.message
              << " (cycle " << watchdogStatus.cycle << ", simulated response "
              << watchdogStatus.responseTimeMs << " ms, limit "
              << watchdogStatus.timeoutThresholdMs << " ms)\n";
}

void printMenu()
{
    std::cout << "\nABS Simulation - one wheel (educational model)\n"
              << "1. Normal braking\n"
              << "2. Moderate braking\n"
              << "3. Emergency braking\n"
              << "4. Sensor failure\n"
              << "5. Invalid sensor data\n"
              << "6. Sensor timeout\n"
              << "7. Watchdog timeout\n"
              << "8. Run all scenarios\n"
              << "9. Exit\n"
              << "Select an option: ";
}
} // namespace

int main()
{
    const std::vector<Scenario> scenarios = allScenarios();

    while (true)
    {
        printMenu();
        int choice = 0;
        if (!(std::cin >> choice))
        {
            if (std::cin.eof())
            {
                std::cout << '\n';
                return 0;
            }
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Please enter a menu number from 1 to 9.\n";
            continue;
        }

        if (choice == 9)
        {
            std::cout << "Exiting ABS Simulation.\n";
            return 0;
        }
        if (choice == 8)
        {
            for (const Scenario& scenario : scenarios)
            {
                runScenario(scenario);
            }
            continue;
        }
        if (choice >= 1 && choice <= static_cast<int>(scenarios.size()))
        {
            runScenario(scenarios[static_cast<std::size_t>(choice - 1)]);
            continue;
        }
        std::cout << "Invalid selection. Please choose 1 through 9.\n";
    }
}
