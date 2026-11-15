#include "Watchdog.h"

Watchdog::Watchdog(std::uint64_t timeoutThresholdMs)
    : timeoutThresholdMs_(timeoutThresholdMs)
{
}

WatchdogStatus Watchdog::monitorCycle(std::uint64_t simulatedResponseTimeMs)
{
    ++cycleCount_;
    const bool timedOut = simulatedResponseTimeMs > timeoutThresholdMs_;
    return {cycleCount_,
            simulatedResponseTimeMs,
            timeoutThresholdMs_,
            timedOut,
            timedOut ? "TIMEOUT (F04)" : "OK"};
}

void Watchdog::reset()
{
    cycleCount_ = 0;
}

std::uint64_t Watchdog::cycleCount() const
{
    return cycleCount_;
}
