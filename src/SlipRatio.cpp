#include "SlipRatio.h"

#include <cmath>

bool SlipCalculationResult::hasValidSlip() const
{
    return status == SlipCalculationStatus::Valid;
}

bool SlipCalculationResult::hasWheelLinearSpeed() const
{
    return status == SlipCalculationStatus::Valid ||
           status == SlipCalculationStatus::LowVehicleSpeed;
}

SlipCalculationResult SlipRatioCalculator::calculate(const SensorData& data) const
{
    if (!data.sensorStatus)
    {
        return {SlipCalculationStatus::SensorUnavailable, 0.0, 0.0,
                "Sensor is unavailable."};
    }

    const SensorValidation validation = Sensor::validate(data);
    if (!validation.valid)
    {
        return {SlipCalculationStatus::InvalidInput, 0.0, 0.0,
                validation.message};
    }

    const double wheelLinearSpeed = data.wheelAngularSpeed * data.wheelRadius;
    if (!std::isfinite(wheelLinearSpeed))
    {
        return {SlipCalculationStatus::InvalidInput, 0.0, 0.0,
                "Wheel linear speed calculation is not finite."};
    }

    if (data.vehicleSpeed <= LOW_SPEED_CUTOFF_MPS)
    {
        return {SlipCalculationStatus::LowVehicleSpeed, wheelLinearSpeed, 0.0,
                "Vehicle speed is at or below the low-speed cutoff."};
    }

    const double slipRatio =
        (data.vehicleSpeed - wheelLinearSpeed) / data.vehicleSpeed;
    if (!std::isfinite(slipRatio))
    {
        return {SlipCalculationStatus::InvalidInput, wheelLinearSpeed, 0.0,
                "Slip ratio calculation is not finite."};
    }

    return {SlipCalculationStatus::Valid, wheelLinearSpeed, slipRatio,
            "Slip ratio calculated."};
}
