#pragma once

#include "Util.h"

/**
 * Initialize elevator motor
 */
void InitializeElevator(U32 elevatorPort);

/**
 * Free elevator memory
 */
void TerminateElevator();

/**
 * Set motor speed from -1.0f to 1.0f
 */
void Elevate(F32 speed);

/**
 * Set global magnitude of elevator motor
 */
void SetElevatorMagnitude(F32 magnitude);

/**
 * Invert the motor
 */
void InvertElevator();

/**
 * Writes motor value to motor
 */
void UpdateElevator();

