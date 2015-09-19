/*
 * ELON.cpp
 *
 *  Created on: Sep 10, 2015
 *      Author: Zaeem
 */

#include "WPILib.h"
#include "Util.h"
#include "Input.h"
#include "Properties.h"
#include "Chassis.h"
#include "Elevator.h"
#include "Action.h"
#include "ThreadSpace.h"
#include "ELON.h"


ELON* elon;

B32 ELON::TeleopRunnerCallback(){
	return IsOperatorControl() && IsEnabled();
}

B32 ELON::TestRunnerCallback(){
	return IsOperatorControl() && IsEnabled();
}

B32 ELON::AutonomousRunnerCallback(){
	return IsOperatorControl() && IsEnabled();
}

void ELON::TeleopExecutableCallback(){

}

void ELON::TestExecutableCallback(){
	//Input processing
	UpdateInput();
	F32 lx = LX(0);
	F32 ly = LY(0);
	F32 rx = RX(0);
	F32 ry = RY(0);
	F32 lt = LT(0);
	F32 rt = RT(0);

	if(START(0)){
		EnableChassis(TRUE);
	}else if(BACK(0)){
		EnableChassis(FALSE);
	}

	ELONDrive(ly, rx);
	Elevate(RB(0) - LB(0));

	F32 chassisMagnitude = SystemMagnitudeInterpolation(MIN_SPEED, DEF_SPEED, MAX_SPEED, rt - lt);
	F32 elevatorMagnitude = Coserp(DEF_SPEED, MAX_SPEED, NormalizeAlpha(rt - lt));

	SetChassisMagnitude(chassisMagnitude);
	SetElevatorMagnitude(chassisMagnitude);

	//Updating subsystems
	UpdateChassis();
	UpdateElevator();
}

void ELON::AutonomousExecutableCallback(){

}


ELON::ELON(){
	//Thread startup
	InitializeThreadSpace();
	fastThread = new Task("FastThread", (FUNCPTR)(&FastThreadRuntime), 100, KiB(5));
	fastThread->Start(FAST_THREAD_HZ);
	StartFastThread();
	PauseFastThread();

	//System startup

	//Input initialization
	InitializeInput(1);
	SetGamepadPorts(0);

	//Chassis initialization
	InitializeChassis(CHASSIS_PORTS, GYRO_PORT, GYRO_SENSITIVITY);

	//Elevator initialization
	InitializeElevator(ELEVATOR_PORT);

}

void ELON::Autonomous(){

}

void ELON::OperatorControl(){
	F64 startTime = SystemTime();



	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	COUT("[ELON] Total Teleoperator time: %dm%.04fs.", totalMinutes, totalSeconds);
}

void ELON::Test(){
	F64 startTime = SystemTime();
	ResumeFastThread();

	CoreThreadRuntime(CORE_THREAD_HZ, (B32_FUNCPTR)(&ELON::TestRunnerCallback),
					 (EXE_FUNCPTR)(&ELON::TestExecutableCallback), this);

	PauseFastThread();
	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	COUT("[ELON] Total Test time: %dm%.04fs.", totalMinutes, totalSeconds);
}

void ELON::Disabled(){

}

ELON::~ELON(){
	//Thread shutdown
	StopFastThread();
	fastThread->Stop();
	TerminateThreadSpace();

	//System shutdown
	TerminateElevator();
	TerminateChassis();
	TerminateInput();
}


I32 main() {
	if (!HALInitialize()){
		std::cerr<<"[ERROR] HAL could not be initialized."<<std::endl;
		return -1;
	}
	HALReport(HALUsageReporting::kResourceType_Language, HALUsageReporting::kLanguage_CPlusPlus);

	elon = new ELON();
	RobotBase::robotSetup(elon);
	return 0;
}
