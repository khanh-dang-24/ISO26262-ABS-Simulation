#ifndef ABS_SIMULATION_WATCHDOG_H
#define ABS_SIMULATION_WATCHDOG_H

#include <cstdint>
#include <string>

struct WatchdogStatus
{
    std::uint64_t cycle = 0;
    std::uint64_t responseTimeMs = 0;
    std::uint64_t timeoutThresholdMs = 0;
    bool timedOut = false;
    std::string message;
};

class Watchdog
{
public:
    explicit Watchdog(std::uint64_t timeoutThresholdMs = 50);

    WatchdogStatus monitorCycle(std::uint64_t simulatedResponseTimeMs);
    void reset();
    std::uint64_t cycleCount() const;

private:
    std::uint64_t timeoutThresholdMs_;
    std::uint64_t cycleCount_ = 0;
};

#endif
