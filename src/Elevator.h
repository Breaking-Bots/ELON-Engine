#ifndef ELON_ELEVATOR_H
#define ELON_ELEVATOR_H

#include "Util.h"

struct ELONMemory;

/**
 * Set motor speed from -1.0f to 1.0f
 */
void Elevate(ELONMemory* memory, F32 speed);

/**
 * Set global magnitude of elevator motor
 */
void SetElevatorMagnitude(ELONMemory* memory, F32 magnitude);

/**
 * Invert the motor
 */
void InvertElevator(ELONMemory* memory);




#endif
