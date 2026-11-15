#ifndef ABS_SIMULATION_SLIP_RATIO_H
#define ABS_SIMULATION_SLIP_RATIO_H

#include "Sensor.h"

#include <string>

enum class SlipCalculationStatus
{
    Valid,
    LowVehicleSpeed,
    SensorUnavailable,
    InvalidInput
};

struct SlipCalculationResult
{
    SlipCalculationStatus status = SlipCalculationStatus::InvalidInput;
    double wheelLinearSpeed = 0.0;
    double slipRatio = 0.0;
    std::string message;

    bool hasValidSlip() const;
    bool hasWheelLinearSpeed() const;
};

class SlipRatioCalculator
{
public:
    static constexpr double LOW_SPEED_CUTOFF_MPS = 0.5;

    SlipCalculationResult calculate(const SensorData& data) const;
};

#endif
