#include "ABSController.h"

#include <cmath>

ABSControllerOutput ABSController::update(
    double brakePedal,
    const SlipCalculationResult& slip,
    bool criticalFault)
{
    if (criticalFault ||
        slip.status == SlipCalculationStatus::InvalidInput ||
        slip.status == SlipCalculationStatus::SensorUnavailable ||
        (slip.status == SlipCalculationStatus::Valid &&
         !std::isfinite(slip.slipRatio)) ||
        !std::isfinite(brakePedal) || brakePedal < 0.0 || brakePedal > 100.0)
    {
        state_ = ABSState::FAULT;
        return {state_, commandFor(state_)};
    }

    if (brakePedal == 0.0)
    {
        state_ = ABSState::STANDBY;
        return {state_, commandFor(state_)};
    }

    if (slip.status == SlipCalculationStatus::LowVehicleSpeed)
    {
        state_ = ABSState::NORMAL_BRAKING;
        return {state_, commandFor(state_)};
    }

    if (!slip.hasValidSlip())
    {
        state_ = ABSState::FAULT;
        return {state_, commandFor(state_)};
    }

    const ABSState startingState =
        state_ == ABSState::FAULT ? ABSState::NORMAL_BRAKING : state_;
    state_ = transitionFrom(startingState, slip.slipRatio);
    return {state_, commandFor(state_)};
}

void ABSController::reset()
{
    state_ = ABSState::STANDBY;
}

ABSState ABSController::state() const
{
    return state_;
}

ABSState ABSController::transitionFrom(ABSState currentState, double slipRatio)
{
    switch (currentState)
    {
    case ABSState::INCREASE:
        if (slipRatio < LOWER_SLIP_THRESHOLD + HYSTERESIS)
        {
            return ABSState::INCREASE;
        }
        if (slipRatio > UPPER_SLIP_THRESHOLD)
        {
            return ABSState::DECREASE;
        }
        return ABSState::HOLD;

    case ABSState::HOLD:
        if (slipRatio < LOWER_SLIP_THRESHOLD - HYSTERESIS)
        {
            return ABSState::INCREASE;
        }
        if (slipRatio > UPPER_SLIP_THRESHOLD + HYSTERESIS)
        {
            return ABSState::DECREASE;
        }
        return ABSState::HOLD;

    case ABSState::DECREASE:
        if (slipRatio > UPPER_SLIP_THRESHOLD - HYSTERESIS)
        {
            return ABSState::DECREASE;
        }
        if (slipRatio < LOWER_SLIP_THRESHOLD)
        {
            return ABSState::INCREASE;
        }
        return ABSState::HOLD;

    case ABSState::STANDBY:
    case ABSState::NORMAL_BRAKING:
    case ABSState::FAULT:
        if (slipRatio < LOWER_SLIP_THRESHOLD)
        {
            return ABSState::INCREASE;
        }
        if (slipRatio <= UPPER_SLIP_THRESHOLD)
        {
            return ABSState::HOLD;
        }
        return ABSState::DECREASE;
    }

    return ABSState::FAULT;
}

BrakePressureCommand ABSController::commandFor(ABSState state)
{
    switch (state)
    {
    case ABSState::INCREASE:
        return BrakePressureCommand::PRESSURE_INCREASE;
    case ABSState::HOLD:
        return BrakePressureCommand::PRESSURE_HOLD;
    case ABSState::DECREASE:
        return BrakePressureCommand::PRESSURE_DECREASE;
    case ABSState::STANDBY:
    case ABSState::NORMAL_BRAKING:
    case ABSState::FAULT:
        return BrakePressureCommand::NO_COMMAND;
    }
    return BrakePressureCommand::NO_COMMAND;
}

const char* ABSController::stateName(ABSState state)
{
    switch (state)
    {
    case ABSState::STANDBY:
        return "STANDBY";
    case ABSState::NORMAL_BRAKING:
        return "NORMAL_BRAKING";
    case ABSState::INCREASE:
        return "INCREASE";
    case ABSState::HOLD:
        return "HOLD";
    case ABSState::DECREASE:
        return "DECREASE";
    case ABSState::FAULT:
        return "FAULT";
    }
    return "UNKNOWN";
}

const char* ABSController::commandName(BrakePressureCommand command)
{
    switch (command)
    {
    case BrakePressureCommand::PRESSURE_INCREASE:
        return "PRESSURE_INCREASE";
    case BrakePressureCommand::PRESSURE_HOLD:
        return "PRESSURE_HOLD";
    case BrakePressureCommand::PRESSURE_DECREASE:
        return "PRESSURE_DECREASE";
    case BrakePressureCommand::NO_COMMAND:
        return "NO_COMMAND";
    }
    return "UNKNOWN";
}
