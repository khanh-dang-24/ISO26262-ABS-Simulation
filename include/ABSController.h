#ifndef ABS_SIMULATION_ABS_CONTROLLER_H
#define ABS_SIMULATION_ABS_CONTROLLER_H

#include "SlipRatio.h"

enum class ABSState
{
    STANDBY,
    NORMAL_BRAKING,
    INCREASE,
    HOLD,
    DECREASE,
    FAULT
};

enum class BrakePressureCommand
{
    PRESSURE_INCREASE,
    PRESSURE_HOLD,
    PRESSURE_DECREASE,
    NO_COMMAND
};

struct ABSControllerOutput
{
    ABSState state = ABSState::STANDBY;
    BrakePressureCommand pressureCommand = BrakePressureCommand::NO_COMMAND;
};

class ABSController
{
public:
    static constexpr double LOWER_SLIP_THRESHOLD = 0.15;
    static constexpr double UPPER_SLIP_THRESHOLD = 0.20;
    static constexpr double HYSTERESIS = 0.02;

    ABSControllerOutput update(double brakePedal,
                               const SlipCalculationResult& slip,
                               bool criticalFault);
    void reset();
    ABSState state() const;

    static const char* stateName(ABSState state);
    static const char* commandName(BrakePressureCommand command);

private:
    static ABSState transitionFrom(ABSState currentState, double slipRatio);
    static BrakePressureCommand commandFor(ABSState state);

    ABSState state_ = ABSState::STANDBY;
};

#endif
