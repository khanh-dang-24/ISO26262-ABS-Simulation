#ifndef ABS_SIMULATION_FAULT_DETECTION_H
#define ABS_SIMULATION_FAULT_DETECTION_H

#include "Sensor.h"

#include <cstdint>
#include <string>
#include <vector>

enum class FaultCode
{
    F01_SENSOR_UNAVAILABLE,
    F02_INVALID_SENSOR_DATA,
    F03_SENSOR_TIMEOUT,
    F04_CONTROLLER_TIMEOUT
};

struct Fault
{
    FaultCode code;
    std::string message;
};

class FaultDetection
{
public:
    explicit FaultDetection(std::uint64_t maximumStaleCycles = 2);

    std::vector<Fault> detect(const SensorData& data,
                              std::uint64_t currentCycle) const;

    static const char* codeName(FaultCode code);
    static const char* messageFor(FaultCode code);

private:
    std::uint64_t maximumStaleCycles_;
};

#endif
