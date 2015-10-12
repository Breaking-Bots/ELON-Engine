/*
 * Source.cpp
 *
 *  Created on: Sep 26, 2015
 *      Author: Zaeem
 */

#include "ELONEngine.h"

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

	Gamepad* gamepad = GetGamepad(input, 0);

	//Input processing
	F32 lx = Analog(gamepad, _LX);
	F32 ly = Analog(gamepad, _LY);
	F32 rx = Analog(gamepad, _RX);
	F32 ry = Analog(gamepad, _RY);
	F32 lt = Analog(gamepad, _LT);
	F32 rt = Analog(gamepad, _RT);

	if(ButtonTapped(gamepad, _START)){
		EnableChassis(memory, !IsChassisEnabled(memory));
	}
	//memory->Cout("%u", DecToBin(Buttons(gamepad)));
	memory->Cout("%.04f  -  %.04f  -  %.04f  -  %.04f  -  %.04f  -  %.04f", lx, ly, rx, ry, lt, rt);

	ELONDrive(memory, ly, rx);
	Elevate(memory, Button(gamepad, _RB) - Button(gamepad, _LB));

	state->chassisState.chassisMagnitude = memory->SystemMagnitudeInterpolation(MIN_SPEED, DEF_SPEED, MAX_SPEED, rt - lt);
	state->elevatorState.elevatorMagnitude = memory->SystemMagnitudeInterpolation(MIN_SPEED, DEF_SPEED, MAX_SPEED, rt - lt);

	SetChassisMagnitude(memory, state->chassisState.chassisMagnitude);
	SetElevatorMagnitude(memory, state->elevatorState.elevatorMagnitude);

}

ELON_CALLBACK(InitAutonomous){

}

ELON_CALLBACK(AutonomousCallback){
	memory->Cout("Wow");
}

ELON_CALLBACK(InitDisabled){

}

ELON_CALLBACK(DisabledCallback){

}

#ifdef __cplusplus
}
#endif
