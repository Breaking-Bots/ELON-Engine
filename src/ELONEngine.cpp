/*
 * Source.cpp
 *
 *  Created on: Sep 26, 2015
 *      Author: Zaeem
 */

#include "ELONEngine.h"

/*******************************************************************
 * Elon Engine			                                           *
 *******************************************************************/

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

	state->chassisState.chassisMagnitude = memory->SystemMagnitudeInterpolation(MIN_SPEED, DEF_SPEED, MAX_SPEED, rt - lt);
	state->elevatorState.elevatorMagnitude = memory->SystemMagnitudeInterpolation(MIN_SPEED, DEF_SPEED, MAX_SPEED, rt - lt);

	SetChassisMagnitude(memory, state->chassisState.chassisMagnitude);
	SetElevatorMagnitude(memory, state->elevatorState.elevatorMagnitude);

	F32 dtheta = state->chassisState.gyroAngleDeg - state->chassisState.lastGyroAngleDeg;
	if(dtheta > 0.1f || dtheta < -0.1f){
		memory->Cout("Gyro: %4.1f degrees", dtheta);
	}

	ELONDrive(memory, ly, rx);
	Elevate(memory, Button(gamepad, _RB) - Button(gamepad, _LB));
}

ELON_CALLBACK(InitAutonomous){

}

ELON_CALLBACK(AutonomousCallback){
	
}

ELON_CALLBACK(InitDisabled){

}

ELON_CALLBACK(DisabledCallback){

}

#ifdef __cplusplus
}
#endif

/*******************************************************************
 * Chassis				                                           *
 *******************************************************************/


intern void SetLeftRightMotorValues(ELONMemory* memory, F32 leftMgntd, F32 rightMgntd){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState* state = &elonState->chassisState;
	if(state->chassisEnabled){
		state->motorValues[0] = (memory->Clamp(leftMgntd * state->chassisMagnitude, -1.0f, 1.0f) * state->invertedMotors[0]);
		state->motorValues[1] = (memory->Clamp(leftMgntd * state->chassisMagnitude, -1.0f, 1.0f) * state->invertedMotors[1]);
		state->motorValues[2] = (memory->Clamp(-rightMgntd * state->chassisMagnitude, -1.0f, 1.0f) * state->invertedMotors[2]);
		state->motorValues[3] = (memory->Clamp(-rightMgntd * state->chassisMagnitude, -1.0f, 1.0f) * state->invertedMotors[3]);
	}
}

intern void SetMotorValues(ELONMemory* memory, F32 motor0, F32 motor1, F32 motor2, F32 motor3){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState* state = &elonState->chassisState;
	if(state->chassisEnabled){
		state->motorValues[0] = memory->Clamp(motor0 * state->chassisMagnitude, -1.0f, 1.0f) * state->invertedMotors[0];
		state->motorValues[1] = memory->Clamp(motor1 * state->chassisMagnitude, -1.0f, 1.0f) * state->invertedMotors[1];
		state->motorValues[2] = memory->Clamp(motor2 * state->chassisMagnitude, -1.0f, 1.0f) * state->invertedMotors[2];
		state->motorValues[3] = memory->Clamp(motor3 * state->chassisMagnitude, -1.0f, 1.0f) * state->invertedMotors[3];
	}
}

void RawDrive(ELONMemory* memory, F32 mgntd, F32 curve){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState* state = &elonState->chassisState;
	if(curve < 0){
		F32 value = logf(-curve);
		F32 ratio = (value - state->sensitivity) / (value + state->sensitivity);
		if(ratio == 0) {
			ratio = EZERO;
		}
		SetLeftRightMotorValues(memory, mgntd/ratio, mgntd);
	}else if(curve > 0){
		F32 value = logf(curve);
		F32 ratio = (value - state->sensitivity) / (value + state->sensitivity);
		if(ratio == 0){
			ratio = EZERO;
		}
		SetLeftRightMotorValues(memory, mgntd, mgntd/ratio);
	}else{
		SetLeftRightMotorValues(memory, mgntd, mgntd);
	}
}

void TankDrive(ELONMemory* memory, F32 leftMgntd, F32 rightMgntd){
	SetLeftRightMotorValues(memory, leftMgntd,rightMgntd);
}

void ELONDrive(ELONMemory* memory, F32 fwdMgntd, F32 turnMgntd){
	fwdMgntd = memory->Clamp(fwdMgntd, -1.0f, 1.0f);
	turnMgntd = memory->Clamp(turnMgntd, -1.0f, 1.0f);
	if(fwdMgntd > 0.0f){
		if(turnMgntd > 0.0f){
			SetLeftRightMotorValues(memory, fwdMgntd - turnMgntd, memory->Max(fwdMgntd, turnMgntd));
		}else{
			SetLeftRightMotorValues(memory, memory->Max(fwdMgntd, -turnMgntd), fwdMgntd + turnMgntd);
		}
	}else{
		if(turnMgntd > 0.0f){
			SetLeftRightMotorValues(memory, -memory->Max(-fwdMgntd, turnMgntd), fwdMgntd + turnMgntd);
		}else{
			SetLeftRightMotorValues(memory, fwdMgntd - turnMgntd, -memory->Max(-fwdMgntd, -turnMgntd));
		}
	}
}

void SetChassisMagnitude(ELONMemory* memory, F32 magnitude){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState* state = &elonState->chassisState;
	state->chassisMagnitude = magnitude;
}

void EnableChassis(ELONMemory* memory, B32 enable){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState* state = &elonState->chassisState;
	state->chassisEnabled = enable;
	if(enable){
		memory->Cout("Chassis enabled");
	}else{
		memory->Cout("Chassis disabled");
	}
}

B32 IsChassisEnabled(ELONMemory* memory){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState* state = &elonState->chassisState;
	return state->chassisEnabled;
}

void StopMotors(ELONMemory* memory){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState* state = &elonState->chassisState;
	if(state->isInitialized){
		for(U32 i = 0; i < state->nMotors; i++){
			state->motorValues[i] = 0.0f;
		}
	}
}

void InvertMotor(ELONMemory* memory, U32 motorPort){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState* state = &elonState->chassisState;
	if(state->isInitialized){
		state->invertedMotors[motorPort] *= -1;
	}
	memory->Cerr("Invalid port: %d.", motorPort);
}

/*******************************************************************
 * Elevator				                                           *
 *******************************************************************/

void Elevate(ELONMemory* memory, F32 speed){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ElevatorState* state = &elonState->elevatorState;
	state->motorValue = memory->Clamp(speed * state->invertedMotor * state->elevatorMagnitude, -1.0f, 1.0f);
}

void SetElevatorMagnitude(ELONMemory* memory, F32 magnitude){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ElevatorState* state = &elonState->elevatorState;
	state->elevatorMagnitude = magnitude;
}

void InvertElevator(ELONMemory* memory){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ElevatorState* state = &elonState->elevatorState;
	state->invertedMotor *= -1;
}

/*******************************************************************
 * Action    			                                           *
 *******************************************************************/



/*******************************************************************
 * Input				                                           *
 *******************************************************************/

U32 Buttons(Gamepad* gamepad){
	U32 buttons = 0;
	for(U32 i = 0; i < NUM_BUTTONS; i++){
		buttons |= buttons & (gamepad->buttons[i].endedDown << i);
	}
	return buttons;
}

B32 Button(Gamepad* gamepad, U32 buttonIndex){
	return (gamepad->buttons[buttonIndex].endedDown);
}

B32 ButtonTapped(Gamepad* gamepad, U32 buttonIndex){
	return (gamepad->buttons[buttonIndex].endedDown && (gamepad->buttons[buttonIndex].halfTransitionCount));
}

B32 ButtonHeld(Gamepad* gamepad, U32 buttonIndex){
	return (gamepad->buttons[buttonIndex].endedDown && !gamepad->buttons[buttonIndex].halfTransitionCount);
}

B32 ButtonReleased(Gamepad* gamepad, U32 buttonIndex){
	return (!gamepad->buttons[buttonIndex].endedDown && (gamepad->buttons[buttonIndex].halfTransitionCount));
}

B32 ButtonPostRealeased(Gamepad* gamepad, U32 buttonIndex){
	return (!gamepad->buttons[buttonIndex].endedDown && !gamepad->buttons[buttonIndex].halfTransitionCount);
}

B32 DPAD(Gamepad* gamepad, U32 dpadIndex){
	return (gamepad->dpad[dpadIndex].endedDown);
}

B32 DPADTapped(Gamepad* gamepad, U32 dpadIndex){
	return (gamepad->dpad[dpadIndex].endedDown && (gamepad->dpad[dpadIndex].halfTransitionCount));
}

B32 DPADHeld(Gamepad* gamepad, U32 dpadIndex){
	return (gamepad->dpad[dpadIndex].endedDown && !gamepad->dpad[dpadIndex].halfTransitionCount);
}

B32 DPADReleased(Gamepad* gamepad, U32 dpadIndex){
	return (!gamepad->dpad[dpadIndex].endedDown && (gamepad->dpad[dpadIndex].halfTransitionCount));
}

B32 DPADPostRealeased(Gamepad* gamepad, U32 dpadIndex){
	return (!gamepad->dpad[dpadIndex].endedDown && !gamepad->dpad[dpadIndex].halfTransitionCount);
}

F32 Analog(Gamepad* gamepad, U32 stickIndex) {
	return gamepad->analog[stickIndex];
}

InputType GetInputType(Gamepad* gamepad){
	return gamepad->inputType;
}

void SetInputType(Gamepad* gamepad, InputType inputType){
	gamepad->inputType = inputType;
}


/*******************************************************************
 * Memory				                                           *
 *******************************************************************/

