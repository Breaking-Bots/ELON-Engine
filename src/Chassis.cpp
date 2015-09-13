/*
 * Chassis.cpp
 *
 *  Created on: Sep 11, 2015
 *      Author: Zaeem
 */

#include "WPILib.h"
#include "Chassis.h"

B32 motorsInitialized = FALSE;

F32 sensitivity = 0.5f;

U32 nMotors; //Number of used motors
SpeedController* motors; //Array of motor controllers
F32* motorValues; //Array of motor speed values
U32* motorPorts; //Array of motor ports
I8* invertedMotors; //Number of motors inverted

F32 chassisMagnitude;
B32 chassisEnabled;



void InitializeChassis(U32 frontLeft, U32 backLeft, U32 frontRight, U32 backRight){
	nMotors = 4;
	motors = new SpeedController[nMotors];
	motorValues = new F32[nMotors];
	motorPorts = new U32[nMotors];
	invertedMotors = new I8[nMotors];

	motorPorts[0] = frontLeft;
	motorPorts[1] = backLeft;
	motorPorts[2] = frontRight;
	motorPorts[3] = backRight;

	for(U32 i = 0; i < nMotors; i++){
			motors[i] = Talon(motorPorts[i]);
			invertedMotors[i] = 1;
	}
	motorsInitialized = TRUE;
	StopMotors();
}

void TerminateChassis(){
	if(invertedMotors){
		delete[] invertedMotors;
	}
	if(motorValues){
		delete[] motorValues;
	}
	if(motorPorts){
		delete[] motorPorts;
	}
	if(motors){
		delete[] motors;
	}
}

intern void SetLeftRightMotorValues(F32 leftMgntd, F32 rightMgntd){
	motorValues[0] = (Clamp(leftMgntd, -1.0f, 1.0f) * invertedMotors[0]);
	motorValues[1] = (Clamp(leftMgntd, -1.0f, 1.0f) * invertedMotors[1]);
	motorValues[2] = (Clamp(rightMgntd, -1.0f, 1.0f) * invertedMotors[2]);
	motorValues[3] = (Clamp(rightMgntd, -1.0f, 1.0f) * invertedMotors[3]);
}

intern void SetMotorValues(F32 motor0, F32 motor1, F32 motor2, F32 motor3){
	motorValues[0] = motor0;
	motorValues[1] = motor1;
	motorValues[2] = motor2;
	motorValues[3] = motor3;
}

void RawDrive(F32 mgntd, F32 curve){
	if(curve < 0){
		F32 value = logf(-curve);
		F32 ratio = (value - sensitivity) / (value + sensitivity);
		if(ratio == 0) {
			ratio = EPSILON;
		}
		SetLeftRightMotorValues(mgntd/ratio, mgntd);
	}else if(curve > 0){
		F32 value = logf(curve);
		F32 ratio = (value - sensitivity) / (value + sensitivity);
		if(ratio == 0){
			ratio = EPSILON;
		}
		SetLeftRightMotorValues(mgntd, mgntd/mgntd);
	}else{
		SetLeftRightMotorValues(mgntd, mgntd);
	}
}

void TankDrive(F32 leftMgntd, F32 rightMgntd){
	SetLeftRightMotorValues(leftMgntd,rightMgntd);
}

void ELONDrive(F32 fwdMgntd, F32 turnMgntd){
	if(fwdMgntd > 0.0f){
		if(turnMgntd > 0.0f){
			SetLeftRightMotorValues(fwdMgntd - turnMgntd, Max(fwdMgntd, turnMgntd));
		}else{
			SetLeftRightMotorValues(Max(fwdMgntd, -turnMgntd), fwdMgntd + turnMgntd);
		}
	}else{
		if(turnMgntd > 0.0f){
			SetLeftRightMotorValues(-Max(-fwdMgntd, turnMgntd), fwdMgntd + turnMgntd);
		}else{
			SetLeftRightMotorValues(fwdMgntd - turnMgntd, -Max(-fwdMgntd, -turnMgntd));
		}
	}
}

void SetChassisMagnitude(F32 magnitude){
	chassisMagnitude = magnitude;
}

void EnableChassis(B32 enable){
	chassisEnabled = enable;
}

void StopMotors(){
	if(motorsInitialized){
		for(U32 i = 0; i < nMotors; i++){
			motors[i].Set(0.0f);
		}
	}
}

void InvertMotor(U32 motorPort){
	if(motorsInitialized){
		for(U32 i = 0; i < nMotors; i++){
			if(motorPorts[i] == motorPort) {
				invertedMotors[i]*=-1; return;
			}
		}
	}
	std::cerr << "[ERROR] Invalid port: " << motorPort << "." << std::endl;
}

void UpdateChassis(){
	if(motorsInitialized && chassisEnabled){
		for(U32 i = 0; i < nMotors; i++){
			motors[i].Set(motorValues[i] * chassisMagnitude);
		}
	}else{
		StopMotors();
	}
}
