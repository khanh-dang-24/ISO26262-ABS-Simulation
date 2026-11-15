#include "ABSController.h"
#include "test_support.h"

namespace
{
SlipCalculationResult slipResult(double value)
{
    return {SlipCalculationStatus::Valid, 1.0, value, "test"};
}
}

void runABSControllerTests(TestRunner& runner)
{
    ABSController controller;

    ABSControllerOutput output = controller.update(30.0, slipResult(0.10), false);
    runner.check("slip below 15 percent selects INCREASE",
                 output.state == ABSState::INCREASE &&
                     output.pressureCommand ==
                         BrakePressureCommand::PRESSURE_INCREASE);

    controller.reset();
    output = controller.update(30.0, slipResult(0.175), false);
    runner.check("slip between 15 and 20 percent selects HOLD",
                 output.state == ABSState::HOLD &&
                     output.pressureCommand == BrakePressureCommand::PRESSURE_HOLD);

    controller.reset();
    output = controller.update(30.0, slipResult(0.25), false);
    runner.check("slip above 20 percent selects DECREASE",
                 output.state == ABSState::DECREASE &&
                     output.pressureCommand ==
                         BrakePressureCommand::PRESSURE_DECREASE);

    controller.reset();
    output = controller.update(30.0, slipResult(0.1499), false);
    runner.check("slip immediately below 15 percent selects INCREASE",
                 output.state == ABSState::INCREASE);
    controller.reset();
    output = controller.update(30.0, slipResult(0.15), false);
    runner.check("slip exactly at 15 percent selects HOLD",
                 output.state == ABSState::HOLD);
    controller.reset();
    output = controller.update(30.0, slipResult(0.20), false);
    runner.check("slip exactly at 20 percent selects HOLD",
                 output.state == ABSState::HOLD);
    controller.reset();
    output = controller.update(30.0, slipResult(0.2001), false);
    runner.check("slip immediately above 20 percent selects DECREASE",
                 output.state == ABSState::DECREASE);

    output = controller.update(0.0, slipResult(0.25), false);
    runner.check("no braking selects STANDBY and no command",
                 output.state == ABSState::STANDBY &&
                     output.pressureCommand == BrakePressureCommand::NO_COMMAND);

    SlipCalculationResult lowSpeed{
        SlipCalculationStatus::LowVehicleSpeed, 0.2, 0.0, "low speed"};
    output = controller.update(30.0, lowSpeed, false);
    runner.check("braking at low speed selects NORMAL_BRAKING",
                 output.state == ABSState::NORMAL_BRAKING);

    controller.reset();
    output = controller.update(30.0, slipResult(0.14), false);
    output = controller.update(30.0, slipResult(0.16), false);
    runner.check("hysteresis retains INCREASE just above lower threshold",
                 output.state == ABSState::INCREASE);
    output = controller.update(30.0, slipResult(0.18), false);
    runner.check("hysteresis transitions INCREASE to HOLD",
                 output.state == ABSState::HOLD);

    output = controller.update(30.0, slipResult(0.10), true);
    runner.check("critical fault selects FAULT with no command",
                 output.state == ABSState::FAULT &&
                     output.pressureCommand == BrakePressureCommand::NO_COMMAND);

    output = controller.update(30.0, slipResult(0.10), false);
    runner.check("controller recovers from FAULT on valid cycle",
                 output.state == ABSState::INCREASE);

    SlipCalculationResult invalid;
    output = controller.update(30.0, invalid, false);
    runner.check("invalid slip cannot enter normal ABS control",
                 output.state == ABSState::FAULT);
}
