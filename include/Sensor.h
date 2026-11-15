#ifndef ABS_SIMULATION_SENSOR_H
#define ABS_SIMULATION_SENSOR_H

#include <cstdint>
#include <string>

struct SensorData
{
    double vehicleSpeed = 0.0;
    double wheelAngularSpeed = 0.0;
    double wheelRadius = 0.32;
    double brakePedal = 0.0;
    bool sensorStatus = true;
    std::uint64_t cycleCounter = 0;
};

struct SensorValidation
{
    bool valid = false;
    std::string message;
};

class Sensor
{
public:
    Sensor() = default;
    explicit Sensor(const SensorData& initialData);

    void update(const SensorData& data);
    const SensorData& read() const;

    static SensorValidation validate(const SensorData& data);

private:
    SensorData data_;
};

#endif
