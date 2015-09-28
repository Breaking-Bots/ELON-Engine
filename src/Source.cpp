/*
 * Source.cpp
 *
 *  Created on: Sep 26, 2015
 *      Author: Zaeem
 */

#include "Util.h"
#include "Memory.h"
#include "Properties.h"
#include "Source.h"
#include "Input.h"
#include "Chassis.h"
#include "Elevator.h"

ELON_CALLBACK(TeleopCallback){

}

ELON_CALLBACK(TestCallback){
	ELONState* state = scast<ELONState*>(memory->permanentStorage);
	if(!memory->isInitialized){
		state->chassisState.chassisMagnitude = DEF_SPEED;
		state->elevatorState.elevatorMagnitude = DEF_SPEED;
		memory->isInitialized = True;
	}

	Gamepad* gamepad = &(state->gamepads[0]);

	//Input processing
	F32 lx = LX(gamepad);
	F32 ly = LY(gamepad);
	F32 rx = RX(gamepad);
	F32 ry = RY(gamepad);
	F32 lt = LT(gamepad);
	F32 rt = RT(gamepad);

	if(STARTTapped(gamepad)){
		EnableChassis(&(state->chassisState), !IsChassisEnabled(&(state->chassisState)));
	}
	COUT("%u", DecToBin(Buttons(gamepad)));

	ELONDrive(&(state->chassisState), ly, rx);
	Elevate(&(state->elevatorState), RB(gamepad) - LB(gamepad));

	state->chassisState.chassisMagnitude = SystemMagnitudeInterpolation(MIN_SPEED, DEF_SPEED, MAX_SPEED, rt - lt);
	state->elevatorState.elevatorMagnitude = SystemMagnitudeInterpolation(MIN_SPEED, DEF_SPEED, MAX_SPEED, rt - lt);

	SetChassisMagnitude(&(state->chassisState), state->chassisState.chassisMagnitude);
	SetElevatorMagnitude(&(state->elevatorState), state->elevatorState.elevatorMagnitude);

}

ELON_CALLBACK(AutonomousCallback){

}

ELON_CALLBACK(DisabledCallback){

}
