#ifndef ABS_SIMULATION_TEST_SUPPORT_H
#define ABS_SIMULATION_TEST_SUPPORT_H

#include <iostream>
#include <string>

class TestRunner
{
public:
    void check(const std::string& name, bool passed)
    {
        if (passed)
        {
            ++passed_;
            std::cout << "PASS: " << name << '\n';
        }
        else
        {
            ++failed_;
            std::cout << "FAIL: " << name << '\n';
        }
    }

    int result() const
    {
        std::cout << "\nTest summary: " << passed_ << " passed, "
                  << failed_ << " failed.\n";
        return failed_ == 0 ? 0 : 1;
    }

private:
    int passed_ = 0;
    int failed_ = 0;
};

void runSensorTests(TestRunner& runner);
void runSlipRatioTests(TestRunner& runner);
void runABSControllerTests(TestRunner& runner);
void runFaultDetectionTests(TestRunner& runner);
void runWatchdogTests(TestRunner& runner);

#endif
