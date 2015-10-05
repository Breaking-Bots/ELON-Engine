/*
 * Source.cpp
 *
 *  Created on: Sep 26, 2015
 *      Author: Zaeem
 */

#include "../inc/ELONEngine.h"
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

ELON_CALLBACK(InitTeleop){

}

ELON_CALLBACK(TeleopCallback){

}

ELON_CALLBACK(InitTest){

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
		EnableChassis(memory, !IsChassisEnabled(memory));
	}
	//memory->Cout("%u", DecToBin(Buttons(gamepad)));

	ELONDrive(memory, ly, rx);
	Elevate(memory, RB(gamepad) - LB(gamepad));

	state->chassisState.chassisMagnitude = memory->SystemMagnitudeInterpolation(MIN_SPEED, DEF_SPEED, MAX_SPEED, rt - lt);
	state->elevatorState.elevatorMagnitude = memory->SystemMagnitudeInterpolation(MIN_SPEED, DEF_SPEED, MAX_SPEED, rt - lt);

	SetChassisMagnitude(memory, state->chassisState.chassisMagnitude);
	SetElevatorMagnitude(memory, state->elevatorState.elevatorMagnitude);

}

ELON_CALLBACK(InitAutonomous){

}

ELON_CALLBACK(AutonomousCallback){
	std::cout << "Wow" << std::endl;
}

ELON_CALLBACK(InitDisabled){

}

ELON_CALLBACK(DisabledCallback){

}

#ifdef __cplusplus
}
#endif
