/*
 * Elevator.cpp
 *
 *  Created on: Sep 13, 2015
 *      Author: Zaeem
 */

#include "WPILib.h"
#include "Elevator.h"
#include "Memory.h"

void Elevate(ElevatorState* state, F32 speed){
	state->motorValue = Clamp(speed * state->invertedMotor, -1.0f, 1.0f);
}

void SetElevatorMagnitude(ElevatorState* state, F32 magnitude){
	state->elevatorMagnitude = magnitude;
}

void InvertElevator(ElevatorState* state){
	state->invertedMotor *= -1;
}
