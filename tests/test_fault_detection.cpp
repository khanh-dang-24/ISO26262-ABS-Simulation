#include "FaultDetection.h"
#include "test_support.h"

void runFaultDetectionTests(TestRunner& runner)
{
    FaultDetection detector(2);
    SensorData data;
    data.vehicleSpeed = 10.0;
    data.wheelAngularSpeed = 20.0;
    data.wheelRadius = 0.32;
    data.brakePedal = 40.0;
    data.sensorStatus = true;
    data.cycleCounter = 5;

    runner.check("fresh valid sensor data has no fault",
                 detector.detect(data, 6).empty());

    data.sensorStatus = false;
    std::vector<Fault> faults = detector.detect(data, 6);
    runner.check("unavailable sensor reports F01",
                 faults.size() == 1 &&
                     faults.front().code == FaultCode::F01_SENSOR_UNAVAILABLE);

    data.sensorStatus = true;
    data.vehicleSpeed = -1.0;
    faults = detector.detect(data, 6);
    runner.check("invalid sensor data reports F02",
                 faults.size() == 1 &&
                     faults.front().code == FaultCode::F02_INVALID_SENSOR_DATA);

    data.vehicleSpeed = 10.0;
    faults = detector.detect(data, 8);
    runner.check("stale sensor data reports F03",
                 faults.size() == 1 &&
                     faults.front().code == FaultCode::F03_SENSOR_TIMEOUT);

    faults = detector.detect(data, 7);
    runner.check("sensor data at stale-cycle boundary remains fresh",
                 faults.empty());

    data.cycleCounter = 9;
    faults = detector.detect(data, 8);
    runner.check("future sensor cycle reports F02",
                 faults.size() == 1 &&
                     faults.front().code == FaultCode::F02_INVALID_SENSOR_DATA);

    runner.check("fault identifiers and messages are readable",
                 std::string(FaultDetection::codeName(
                     FaultCode::F03_SENSOR_TIMEOUT)) == "F03" &&
                     std::string(FaultDetection::messageFor(
                         FaultCode::F03_SENSOR_TIMEOUT)).size() > 0);
}
