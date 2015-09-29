/*
 * Chassis.cpp
 *
 *  Created on: Sep 11, 2015
 *      Author: Zaeem
 */

#include "ELONEngine.h"

intern void SetLeftRightMotorValues(ELONMemory* memory, F32 leftMgntd, F32 rightMgntd){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState state = elonState->chassisState;
	state.motorValues[0] = (memory->Clamp(leftMgntd, -1.0f, 1.0f) * state.invertedMotors[0]);
	state.motorValues[1] = (memory->Clamp(leftMgntd, -1.0f, 1.0f) * state.invertedMotors[1]);
	state.motorValues[2] = (memory->Clamp(-rightMgntd, -1.0f, 1.0f) * state.invertedMotors[2]);
	state.motorValues[3] = (memory->Clamp(-rightMgntd, -1.0f, 1.0f) * state.invertedMotors[3]);
}

intern void SetMotorValues(ELONMemory* memory, F32 motor0, F32 motor1, F32 motor2, F32 motor3){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState state = elonState->chassisState;
	state.motorValues[0] = motor0;
	state.motorValues[1] = motor1;
	state.motorValues[2] = motor2;
	state.motorValues[3] = motor3;
}

void RawDrive(ELONMemory* memory, F32 mgntd, F32 curve){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState state = elonState->chassisState;
	if(curve < 0){
		F32 value = logf(-curve);
		F32 ratio = (value - state.sensitivity) / (value + state.sensitivity);
		if(ratio == 0) {
			ratio = EZERO;
		}
		SetLeftRightMotorValues(memory, mgntd/ratio, mgntd);
	}else if(curve > 0){
		F32 value = logf(curve);
		F32 ratio = (value - state.sensitivity) / (value + state.sensitivity);
		if(ratio == 0){
			ratio = EZERO;
		}
		SetLeftRightMotorValues(memory, mgntd, mgntd/ratio);
	}else{
		SetLeftRightMotorValues(memory, mgntd, mgntd);
	}
}

void TankDrive(ELONMemory* memory, F32 leftMgntd, F32 rightMgntd){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	SetLeftRightMotorValues(memory, leftMgntd,rightMgntd);
}

void ELONDrive(ELONMemory* memory, F32 fwdMgntd, F32 turnMgntd){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
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
	ChassisState state = elonState->chassisState;
	state.chassisMagnitude = magnitude;
}

void EnableChassis(ELONMemory* memory, B32 enable){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState state = elonState->chassisState;
	state.chassisEnabled = enable;
	if(enable){
		memory->Cout("Chassis enabled");
	}else{
		memory->Cout("Chassis disabled");
	}
}

B32 IsChassisEnabled(ELONMemory* memory){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState state = elonState->chassisState;
	return state.chassisEnabled;
}

void StopMotors(ELONMemory* memory){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState state = elonState->chassisState;
	if(state.isInitialized){
		for(U32 i = 0; i < state.nMotors; i++){
			state.motorValues[i] = 0.0f;
		}
	}
}

void InvertMotor(ELONMemory* memory, U32 motorPort){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState state = elonState->chassisState;
	if(state.isInitialized){
		state.invertedMotors[motorPort] *= -1;
	}
	memory->Cerr("Invalid port: %d.", motorPort);
}
