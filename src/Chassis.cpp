/*
 * Chassis.cpp
 *
 *  Created on: Sep 11, 2015
 *      Author: Zaeem
 */

#include "WPILib.h"
#include "Chassis.h"
#include "Memory.h"

intern void SetLeftRightMotorValues(ChassisState* state, F32 leftMgntd, F32 rightMgntd){
	state->motorValues[0] = (Clamp(leftMgntd, -1.0f, 1.0f) * state->invertedMotors[0]);
	state->motorValues[1] = (Clamp(leftMgntd, -1.0f, 1.0f) * state->invertedMotors[1]);
	state->motorValues[2] = (Clamp(-rightMgntd, -1.0f, 1.0f) * state->invertedMotors[2]);
	state->motorValues[3] = (Clamp(-rightMgntd, -1.0f, 1.0f) * state->invertedMotors[3]);
}

intern void SetMotorValues(ChassisState* state, F32 motor0, F32 motor1, F32 motor2, F32 motor3){
	state->motorValues[0] = motor0;
	state->motorValues[1] = motor1;
	state->motorValues[2] = motor2;
	state->motorValues[3] = motor3;
}

void RawDrive(ChassisState* state, F32 mgntd, F32 curve){
	if(curve < 0){
		F32 value = logf(-curve);
		F32 ratio = (value - state->sensitivity) / (value + state->sensitivity);
		if(ratio == 0) {
			ratio = EZERO;
		}
		SetLeftRightMotorValues(state, mgntd/ratio, mgntd);
	}else if(curve > 0){
		F32 value = logf(curve);
		F32 ratio = (value - state->sensitivity) / (value + state->sensitivity);
		if(ratio == 0){
			ratio = EZERO;
		}
		SetLeftRightMotorValues(state, mgntd, mgntd/ratio);
	}else{
		SetLeftRightMotorValues(state, mgntd, mgntd);
	}
}

void TankDrive(ChassisState* state, F32 leftMgntd, F32 rightMgntd){
	SetLeftRightMotorValues(state, leftMgntd,rightMgntd);
}

void ELONDrive(ChassisState* state, F32 fwdMgntd, F32 turnMgntd){
	fwdMgntd = Clamp(fwdMgntd, -1.0f, 1.0f);
	turnMgntd = Clamp(turnMgntd, -1.0f, 1.0f);
	if(fwdMgntd > 0.0f){
		if(turnMgntd > 0.0f){
			SetLeftRightMotorValues(state, fwdMgntd - turnMgntd, Max(fwdMgntd, turnMgntd));
		}else{
			SetLeftRightMotorValues(state, Max(fwdMgntd, -turnMgntd), fwdMgntd + turnMgntd);
		}
	}else{
		if(turnMgntd > 0.0f){
			SetLeftRightMotorValues(state, -Max(-fwdMgntd, turnMgntd), fwdMgntd + turnMgntd);
		}else{
			SetLeftRightMotorValues(state, fwdMgntd - turnMgntd, -Max(-fwdMgntd, -turnMgntd));
		}
	}
}

void SetChassisMagnitude(ChassisState* state, F32 magnitude){
	state->chassisMagnitude = magnitude;
}

void EnableChassis(ChassisState* state, B32 enable){
	state->chassisEnabled = enable;
	if(enable){
		COUT("Chassis enabled");
	}else{
		COUT("Chassis disabled");
	}
}

B32 IsChassisEnabled(ChassisState* state){
	return state->chassisEnabled;
}

void StopMotors(ChassisState* state){
	if(state->isInitialized){
		for(U32 i = 0; i < state->nMotors; i++){
			state->motorValues[i] = 0.0f;
		}
	}
}

void InvertMotor(ChassisState* state, U32 motorPort){
	if(state->isInitialized){
		state->invertedMotors[motorPort] *= -1;
	}
	CERR("Invalid port: %d.", motorPort);
}
