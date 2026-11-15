#include "Sensor.h"
#include "test_support.h"

#include <limits>

void runSensorTests(TestRunner& runner)
{
    SensorData valid;
    valid.vehicleSpeed = 12.0;
    valid.wheelAngularSpeed = 30.0;
    valid.wheelRadius = 0.32;
    valid.brakePedal = 45.0;
    valid.sensorStatus = true;
    valid.cycleCounter = 4;

    runner.check("sensor accepts valid input", Sensor::validate(valid).valid);

    SensorData invalid = valid;
    invalid.wheelAngularSpeed = -1.0;
    runner.check("sensor rejects negative wheel speed",
                 !Sensor::validate(invalid).valid);

    invalid = valid;
    invalid.vehicleSpeed = std::numeric_limits<double>::quiet_NaN();
    runner.check("sensor rejects non-finite input",
                 !Sensor::validate(invalid).valid);

    invalid = valid;
    invalid.wheelRadius = 0.0;
    runner.check("sensor rejects zero wheel radius",
                 !Sensor::validate(invalid).valid);

    invalid = valid;
    invalid.wheelAngularSpeed = std::numeric_limits<double>::max();
    invalid.wheelRadius = std::numeric_limits<double>::max();
    runner.check("sensor rejects non-finite derived wheel speed",
                 !Sensor::validate(invalid).valid);

    invalid = valid;
    invalid.brakePedal = 101.0;
    runner.check("sensor rejects brake pedal outside range",
                 !Sensor::validate(invalid).valid);

    Sensor sensor;
    sensor.update(valid);
    runner.check("sensor stores configurable input and cycle",
                 sensor.read().vehicleSpeed == 12.0 &&
                     sensor.read().cycleCounter == 4);
}
