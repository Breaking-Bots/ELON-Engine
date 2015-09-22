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
#include "Actions.h"
#include "ThreadSpace.h"
#include "Memory.h"
#include "ELON.h"


ELON* elon;

void TeleopExecutableCallback(ELONMemory* memory){

}

void TestExecutableCallback(ELONMemory* memory){
	ELONState* state = (ELONState*)memory->permanentStorage;
	if(!memory->isInitialized){
		state->chassisMagnitude = DEF_SPEED;
		state->elevatorMagnitude = DEF_SPEED;
		memory->isInitialized = TRUE;
	}

	//Input processing
	F32 lx = LX(0);
	F32 ly = LY(0);
	F32 rx = RX(0);
	F32 ry = RY(0);
	F32 lt = LT(0);
	F32 rt = RT(0);

	if(STARTTapped(0)){
		EnableChassis(!IsChassisEnabled());
	}
	//COUT("%u", DecToBin(Buttons(0)));

	ELONDrive(ly, rx);
	Elevate(RB(0) - LB(0));

	state->chassisMagnitude = SystemMagnitudeInterpolation(MIN_SPEED, DEF_SPEED, MAX_SPEED, rt - lt);
	state->elevatorMagnitude = SystemMagnitudeInterpolation(MIN_SPEED, DEF_SPEED, MAX_SPEED, rt - lt);

	SetChassisMagnitude(state->chassisMagnitude);
	SetElevatorMagnitude(state->elevatorMagnitude);

	//Updating subsystems
	UpdateChassis();
	UpdateElevator();
}

void AutonomousExecutableCallback(ELONMemory* memory){

}


ELON::ELON(){
	COUT("Initializing");
	//Memory startup
	elonMemory = {};
	elonMemory.permanentStorageSize = ELON_PERMANENT_STORAGE_SIZE;
	elonMemory.permanentStorage = malloc(elonMemory.permanentStorageSize);
	if(elonMemory.permanentStorage){
		COUT("Permanent memory successfully allocated with size of %u Bytes", ELON_PERMANENT_STORAGE_SIZE);
	}else{
		CERR("Permanent memory allocation failed with size of %u Bytes", ELON_PERMANENT_STORAGE_SIZE);
	}
	memset(elonMemory.permanentStorage, 0, elonMemory.permanentStorageSize);
	elonMemory.transientStorageSize = ELON_TRANSIENT_STORAGE_SIZE;
	elonMemory.transientStorage = malloc(elonMemory.transientStorageSize);
	if(elonMemory.transientStorage){
		COUT("Transient memory successfully allocated with size of %u Bytes", ELON_TRANSIENT_STORAGE_SIZE);
	}else{
		CERR("Transient memory allocation failed with size of %u Bytes", ELON_TRANSIENT_STORAGE_SIZE);
	}
	memset(elonMemory.transientStorage, 0, elonMemory.transientStorageSize);

	//Thread startup
	InitializeThreadSpace();
	fastThread = new Task("FastThread", (FUNCPTR)(&FastThreadRuntime), 100, KiB(5));
	fastThread->Start(FAST_THREAD_HZ);
	StartFastThread();
	PauseFastThread();
}

void ELON::RobotInit(){

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
	F64 startTime = SystemTime();
	ResumeFastThread();

	CoreThreadRuntime(CORE_THREAD_HZ, (B32_FUNCPTR)(&ELON::IsAutonomous),
					 (EXE_FUNCPTR)(&AutonomousExecutableCallback), this);

	PauseFastThread();
	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	COUT("Total Autonomous time: %dm%.04fs.", totalMinutes, totalSeconds);
}

void ELON::OperatorControl(){
	F64 startTime = SystemTime();
	ResumeFastThread();

	CoreThreadRuntime(CORE_THREAD_HZ, (B32_FUNCPTR)(&ELON::IsOperatorControl),
					 (EXE_FUNCPTR)(&TeleopExecutableCallback), this);

	PauseFastThread();
	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) / 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	COUT("Total Teleoperator time: %dm%.04fs.", totalMinutes, totalSeconds);
}

void ELON::Test(){
	F64 startTime = SystemTime();
	ResumeFastThread();

	CoreThreadRuntime(CORE_THREAD_HZ, (B32_FUNCPTR)(&ELON::IsTest),
					 (EXE_FUNCPTR)(&TestExecutableCallback), this);

	PauseFastThread();
	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	COUT("Total Test time: %dm%.04fs.", totalMinutes, totalSeconds);
}

void ELON::Disabled(){

}

ELON::~ELON(){
	//System shutdown
	TerminateElevator();
	TerminateChassis();
	TerminateInput();

	//Thread shutdown
	StopFastThread();
	fastThread->Stop();
	TerminateThreadSpace();

	//Memory shutdown
	free(elonMemory.transientStorage);
	free(elonMemory.permanentStorage);

	//Logging shutdown
}


I32 main() {
	//Loggign startup
	InitializeLogging();
	if (!HALInitialize()){
		CERR("HAL could not be initialized.");
		return -1;
	}
	HALReport(HALUsageReporting::kResourceType_Language, HALUsageReporting::kLanguage_CPlusPlus);

	elon = new ELON();
	RobotBase::robotSetup(elon);
	return 0;
}
