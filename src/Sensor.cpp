#include "Sensor.h"

#include <cmath>

Sensor::Sensor(const SensorData& initialData)
    : data_(initialData)
{
}

void Sensor::update(const SensorData& data)
{
    data_ = data;
}

const SensorData& Sensor::read() const
{
    return data_;
}

SensorValidation Sensor::validate(const SensorData& data)
{
    if (!std::isfinite(data.vehicleSpeed) ||
        !std::isfinite(data.wheelAngularSpeed) ||
        !std::isfinite(data.wheelRadius) ||
        !std::isfinite(data.brakePedal))
    {
        return {false, "Sensor values must all be finite."};
    }

    if (data.vehicleSpeed < 0.0 || data.wheelAngularSpeed < 0.0)
    {
        return {false, "Vehicle and wheel speeds must be non-negative."};
    }

    if (data.wheelRadius <= 0.0)
    {
        return {false, "Wheel radius must be greater than zero."};
    }

    if (!std::isfinite(data.wheelAngularSpeed * data.wheelRadius))
    {
        return {false, "Wheel linear speed calculation is not finite."};
    }

    if (data.brakePedal < 0.0 || data.brakePedal > 100.0)
    {
        return {false, "Brake pedal input must be between 0 and 100 percent."};
    }

    return {true, "Sensor data is valid."};
}
