/*
 * Chassis.cpp
 *
 *  Created on: Sep 11, 2015
 *      Author: Zaeem
 */

#include "WPILib.h"
#include "Chassis.h"
#include <cmath>

B32 motorsInitialized = FALSE;

F32 sensitivity = 0.5f;

U32 nMotors; //Number of used motors
SpeedController* motors; //Array of motor controllers
U32* motorPorts; //Array of motor ports
I8* invertedMotors; //Number of motors inverted

F32 chassisMagnitude;
B32 chassisEnabled;

void InitializeChassis(U32 numMotors){
	nMotors = numMotors;
	motors = new SpeedController[nMotors];
	motorPorts = new U32[nMotors];
	invertedMotors = new I8[nMotors];
}

void SetMotorPorts(U32 left, U32 right){
	if(nMotors == 2){
		motorPorts[0] = left;
		motorPorts[1] = right;
	}else{
		std::cerr << "Amount of motor ports do not match number of motors: " << nMotors << "." << std::endl;
	}
}

void SetMotorPorts(U32 frontLeft, U32 backLeft, U32 frontRight, U32 backRight){
	if(nMotors == 4){
		motorPorts[0] = frontLeft;
		motorPorts[1] = backLeft;
		motorPorts[2] = frontRight;
		motorPorts[3] = backRight;
	}else{
		std::cerr << "Amount of motor ports do not match number of motors: " << nMotors << "." << std::endl;
	}
}

void InitializeMotors(){
	for(U32 i = 0; i < nMotors; i++){
		motors[i] = new Talon(motorPorts[i]);
		invertedMotors[i] = 1;
	}
	motorsInitialized = TRUE;
	StopMotors();
}

void TerminateChassis(){
	if(invertedMotors){
		delete[] invertedMotors;
	}
	if(motorPorts){
		delete[] motorPorts;
	}
	if(motors){
		delete[] motors;
	}
}


intern void SetLeftRightMotorOutputs(F32 leftMgntd, F32 rightMgntd){
	if(motorsInitialized && chassisEnabled){
		if(nMotors == 4){
			motors[0].Set(Clamp(leftMgntd, -1.0f, 1.0f) * invertedMotors[0] * chassisMagnitude);
			motors[1].Set(Clamp(leftMgntd, -1.0f, 1.0f) * invertedMotors[1] * chassisMagnitude);
			motors[2].Set(Clamp(rightMgntd, -1.0f, 1.0f) * invertedMotors[2] * chassisMagnitude);
			motors[3].Set(Clamp(rightMgntd, -1.0f, 1.0f) * invertedMotors[3] * chassisMagnitude);
		}else if(nMotors == 2){
			motors[0].Set(Clamp(leftMgntd, -1.0f, 1.0f) * invertedMotors[0] * chassisMagnitude);
			motors[1].Set(Clamp(rightMgntd, -1.0f, 1.0f) * invertedMotors[1] * chassisMagnitude);
		}
	}
}

/** Mainly used in Autonomous
 * mgntd - forward component of magnitude sent to motors
 * curve - rate of turn, constant for differing forward speeds
 */
void RawDrive(F32 mgntd, F32 curve){
	if(curve < 0){
		F32 value = logf(-curve);
		F32 ratio = (value - sensitivity) / (value + sensitivity);
		if(ratio == 0) {
			ratio = EPSILON;
		}
		SetLeftRightMotorOutputs(mgntd/ratio, mgntd);
	}else if(curve > 0){
		F32 value = logf(curve);
		F32 ratio = (value - sensitivity) / (value + sensitivity);
		if(ratio == 0){
			ratio = EPSILON;
		}
		SetLeftRightMotorOutputs(mgntd, mgntd/mgntd);
	}else{
		SetLeftRightMotorOutputs(mgntd, mgntd);
	}
}

/**
 * leftMgntd - magnitude of left motors
 * rightMgntd - magnitude of right motors
 */
void TankDrive(F32 leftMgntd, F32 rightMgntd){
	SetLeftRightMotorOutputs(leftMgntd,rightMgntd);
}

/**
 * fwdMgntd - magnitude of robot moving forward
 * turnMgntd - magnitude of turning of robot
 */
void ELONDrive(F32 fwdMgntd, F32 turnMgntd){
	if(fwdMgntd > 0.0f){
		if(turnMgntd > 0.0f){
			SetLeftRightMotorOutputs(fwdMgntd - turnMgntd, Max(fwdMgntd, turnMgntd));
		}else{
			SetLeftRightMotorOutputs(Max(fwdMgntd, -turnMgntd), fwdMgntd + turnMgntd);
		}
	}else{
		if(turnMgntd > 0.0f){
			SetLeftRightMotorOutputs(-Max(-fwdMgntd, turnMgntd), fwdMgntd + turnMgntd);
		}else{
			SetLeftRightMotorOutputs(fwdMgntd - turnMgntd, -Max(-fwdMgntd, -turnMgntd));
		}
	}
}

/**
 * Sets global magnitude of all motors
 */
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
	std::cerr << "Invalid port: " << motorPort << "." << std::endl;
}

