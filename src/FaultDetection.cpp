#include "FaultDetection.h"

FaultDetection::FaultDetection(std::uint64_t maximumStaleCycles)
    : maximumStaleCycles_(maximumStaleCycles)
{
}

std::vector<Fault> FaultDetection::detect(const SensorData& data,
                                          std::uint64_t currentCycle) const
{
    if (!data.sensorStatus)
    {
        return {{FaultCode::F01_SENSOR_UNAVAILABLE,
                 messageFor(FaultCode::F01_SENSOR_UNAVAILABLE)}};
    }

    const SensorValidation validation = Sensor::validate(data);
    if (!validation.valid || data.cycleCounter > currentCycle)
    {
        return {{FaultCode::F02_INVALID_SENSOR_DATA,
                 validation.valid ? "Sensor cycle counter is ahead of controller."
                                  : messageFor(FaultCode::F02_INVALID_SENSOR_DATA)}};
    }

    if (currentCycle - data.cycleCounter > maximumStaleCycles_)
    {
        return {{FaultCode::F03_SENSOR_TIMEOUT,
                 messageFor(FaultCode::F03_SENSOR_TIMEOUT)}};
    }

    return {};
}

const char* FaultDetection::codeName(FaultCode code)
{
    switch (code)
    {
    case FaultCode::F01_SENSOR_UNAVAILABLE:
        return "F01";
    case FaultCode::F02_INVALID_SENSOR_DATA:
        return "F02";
    case FaultCode::F03_SENSOR_TIMEOUT:
        return "F03";
    case FaultCode::F04_CONTROLLER_TIMEOUT:
        return "F04";
    }
    return "UNKNOWN";
}

const char* FaultDetection::messageFor(FaultCode code)
{
    switch (code)
    {
    case FaultCode::F01_SENSOR_UNAVAILABLE:
        return "Sensor unavailable.";
    case FaultCode::F02_INVALID_SENSOR_DATA:
        return "Invalid sensor data.";
    case FaultCode::F03_SENSOR_TIMEOUT:
        return "Sensor data is stale or timed out.";
    case FaultCode::F04_CONTROLLER_TIMEOUT:
        return "Controller execution timeout detected by watchdog.";
    }
    return "Unknown fault.";
}
