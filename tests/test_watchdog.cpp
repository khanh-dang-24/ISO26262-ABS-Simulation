#include "Watchdog.h"
#include "test_support.h"

void runWatchdogTests(TestRunner& runner)
{
    Watchdog watchdog(50);
    WatchdogStatus status = watchdog.monitorCycle(20);
    runner.check("watchdog accepts response below timeout",
                 !status.timedOut && status.cycle == 1);

    status = watchdog.monitorCycle(50);
    runner.check("watchdog accepts response at timeout boundary",
                 !status.timedOut && status.cycle == 2);

    status = watchdog.monitorCycle(51);
    runner.check("watchdog detects response above timeout",
                 status.timedOut && status.message == "TIMEOUT (F04)" &&
                     status.cycle == 3);

    watchdog.reset();
    status = watchdog.monitorCycle(0);
    runner.check("watchdog reset clears cycle monitoring",
                 !status.timedOut && status.cycle == 1);
}
