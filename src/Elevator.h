#ifndef ELON_ELEVATOR_H
#define ELON_ELEVATOR_H

#include "Util.h"

struct ElevatorState;

/**
 * Set motor speed from -1.0f to 1.0f
 */
void Elevate(ElevatorState* state, F32 speed);

/**
 * Set global magnitude of elevator motor
 */
void SetElevatorMagnitude(ElevatorState* state, F32 magnitude);

/**
 * Invert the motor
 */
void InvertElevator(ElevatorState* state);




#endif
