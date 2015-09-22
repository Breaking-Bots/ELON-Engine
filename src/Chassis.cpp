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
SpeedController** motors; //Array of motor controllers
F32* motorValues; //Array of motor speed values
U32* motorPorts; //Array of motor ports
I8* invertedMotors; //Number of motors inverted
Gyro* gyro; //Gyroscope

F32 chassisMagnitude;
B32 chassisEnabled;

MUTEX_ID motorLock;
MUTEX_ID motorValueLock;
MUTEX_ID invertedMotorLock;

void InitializeChassis(U32 frontLeft, U32 backLeft, U32 frontRight, U32 backRight, U32 gyroPort, U32 gyroSensitivity){
	motorLock = initializeMutexNormal();
	motorValueLock = initializeMutexNormal();
	invertedMotorLock = initializeMutexNormal();
	nMotors = 4;
	motors = new SpeedController*[nMotors];
	motorValues = new F32[nMotors];
	motorPorts = new U32[nMotors];
	invertedMotors = new I8[nMotors];
	gyro = new Gyro(gyroPort);
	gyro->SetSensitivity(gyroSensitivity);

	motorPorts[0] = frontLeft;
	motorPorts[1] = backLeft;
	motorPorts[2] = frontRight;
	motorPorts[3] = backRight;

	for(U32 i = 0; i < nMotors; i++){
			motors[i] = new Talon(motorPorts[i]);
			invertedMotors[i] = -1;
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
		for(U32 i = 0; i < nMotors; i++){
			if(motors[i]){
				delete motors[i];
			}
		}
		delete[] motors;
	}
	deleteMutex(invertedMotorLock);
	deleteMutex(motorValueLock);
	deleteMutex(motorLock);
}

intern void SetLeftRightMotorValues(F32 leftMgntd, F32 rightMgntd){
	CRITICAL_REGION(motorValueLock);
		motorValues[0] = (Clamp(leftMgntd, -1.0f, 1.0f) * invertedMotors[0]);
		motorValues[1] = (Clamp(leftMgntd, -1.0f, 1.0f) * invertedMotors[1]);
		motorValues[2] = (Clamp(-rightMgntd, -1.0f, 1.0f) * invertedMotors[2]);
		motorValues[3] = (Clamp(-rightMgntd, -1.0f, 1.0f) * invertedMotors[3]);
	END_REGION;
}

intern void SetMotorValues(F32 motor0, F32 motor1, F32 motor2, F32 motor3){
	CRITICAL_REGION(motorValueLock);
		motorValues[0] = motor0;
		motorValues[1] = motor1;
		motorValues[2] = motor2;
		motorValues[3] = motor3;
	END_REGION;
}

void RawDrive(F32 mgntd, F32 curve){
	if(curve < 0){
		F32 value = logf(-curve);
		F32 ratio = (value - sensitivity) / (value + sensitivity);
		if(ratio == 0) {
			ratio = EZERO;
		}
		SetLeftRightMotorValues(mgntd/ratio, mgntd);
	}else if(curve > 0){
		F32 value = logf(curve);
		F32 ratio = (value - sensitivity) / (value + sensitivity);
		if(ratio == 0){
			ratio = EZERO;
		}
		SetLeftRightMotorValues(mgntd, mgntd/ratio);
	}else{
		SetLeftRightMotorValues(mgntd, mgntd);
	}
}

void TankDrive(F32 leftMgntd, F32 rightMgntd){
	SetLeftRightMotorValues(leftMgntd,rightMgntd);
}

void ELONDrive(F32 fwdMgntd, F32 turnMgntd){
	fwdMgntd = Clamp(fwdMgntd, -1.0f, 1.0f);
	turnMgntd = Clamp(turnMgntd, -1.0f, 1.0f);
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
	if(enable){
		COUT("Chassis enabled");
	}else{
		COUT("Chassis disabled");
	}
}

B32 IsChassisEnabled(){
	return chassisEnabled;
}

void StopMotors(){
	CRITICAL_REGION(motorLock);
		if(motorsInitialized){
			for(U32 i = 0; i < nMotors; i++){
				motors[i]->Set(0.0f);
			}
		}
	END_REGION;
}

void InvertMotor(U32 motorPort){
	if(motorsInitialized){
		CRITICAL_REGION(invertedMotorLock);
			for(U32 i = 0; i < nMotors; i++){
				if(motorPorts[i] == motorPort) {
					invertedMotors[i]*=-1;
					return;
				}
			}
		END_REGION;
	}
	CERR("Invalid port: %d.", motorPort);
}

F32 HeadingDeg(){
	return gyro->GetAngle();
}

F32 HeadingRad(){
	return gyro->GetAngle() * DEG_TO_RAD;
}

void UpdateChassis(){
	if(motorsInitialized && chassisEnabled){
		CRITICAL_REGION(motorLock);
			Synchronized __sync(motorValueLock);
			for(U32 i = 0; i < nMotors; i++){
				motors[i]->Set(motorValues[i] * chassisMagnitude);
			}
		END_REGION;
	}else{
		StopMotors();
	}
}
