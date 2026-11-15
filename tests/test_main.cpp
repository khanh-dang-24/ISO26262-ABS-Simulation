#include "test_support.h"

int main()
{
    TestRunner runner;
    runSensorTests(runner);
    runSlipRatioTests(runner);
    runABSControllerTests(runner);
    runFaultDetectionTests(runner);
    runWatchdogTests(runner);
    return runner.result();
}
