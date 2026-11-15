#include "SlipRatio.h"
#include "test_support.h"

#include <cmath>

void runSlipRatioTests(TestRunner& runner)
{
    SlipRatioCalculator calculator;
    SensorData data;
    data.vehicleSpeed = 10.0;
    data.wheelAngularSpeed = 10.0;
    data.wheelRadius = 0.32;
    data.brakePedal = 20.0;

    SlipCalculationResult result = calculator.calculate(data);
    runner.check("wheel linear speed calculation is correct",
                 result.hasWheelLinearSpeed() &&
                     std::abs(result.wheelLinearSpeed - 3.2) < 1e-12);

    data.wheelAngularSpeed = 25.0;
    result = calculator.calculate(data);
    runner.check("slip ratio calculation is correct",
                 result.hasValidSlip() &&
                     std::abs(result.slipRatio - 0.2) < 1e-12);

    data.vehicleSpeed = 0.0;
    result = calculator.calculate(data);
    runner.check("zero vehicle speed does not divide by zero",
                 result.status == SlipCalculationStatus::LowVehicleSpeed &&
                     result.hasWheelLinearSpeed() && !result.hasValidSlip());

    data.vehicleSpeed = 0.5;
    result = calculator.calculate(data);
    runner.check("speed at cutoff is handled as low speed",
                 result.status == SlipCalculationStatus::LowVehicleSpeed);

    data.vehicleSpeed = 0.5001;
    result = calculator.calculate(data);
    runner.check("speed above cutoff calculates slip",
                 result.status == SlipCalculationStatus::Valid);

    data.wheelAngularSpeed = -1.0;
    result = calculator.calculate(data);
    runner.check("slip calculator rejects invalid sensor input",
                 result.status == SlipCalculationStatus::InvalidInput);

    data.wheelAngularSpeed = 1.0;
    data.sensorStatus = false;
    result = calculator.calculate(data);
    runner.check("slip calculator rejects unavailable sensor",
                 result.status == SlipCalculationStatus::SensorUnavailable);
}
