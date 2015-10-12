/*
 * Elevator.cpp
 *
 *  Created on: Sep 13, 2015
 *      Author: Zaeem
 */

#include "ELONEngine.h"

void Elevate(ELONMemory* memory, F32 speed){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ElevatorState state = elonState->elevatorState;
	state.motorValue = memory->Clamp(speed * state.invertedMotor, -1.0f, 1.0f);
}

void SetElevatorMagnitude(ELONMemory* memory, F32 magnitude){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ElevatorState state = elonState->elevatorState;
	state.elevatorMagnitude = magnitude;
}

void InvertElevator(ELONMemory* memory){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ElevatorState state = elonState->elevatorState;
	state.invertedMotor *= -1;
}
