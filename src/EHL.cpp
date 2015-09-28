/*
 * EHL.cpp
 *
 * ELON Hardware Layer
 *
 *  Created on: Sep 10, 2015
 *      Author: Zaeem
 */

#include "WPILib.h"
#include <sys/mman.h>
#include <dlfcn.h>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include "EHL.h"

//TODO: Move
intern F64 SystemTime(){
	return GetFPGATime() / 1000.0;
}

/*******************************************************************
 * ELON Engine Management                                          *
 *******************************************************************/

struct ELONEngine{
	ELONCallback* TeleopCallback;
	ELONCallback* TestCallback;
	ELONCallback* AutonomousCallback;
	ELONCallback* DisabledCallback;
};

intern ELONEngine LoadELONEngine(){
	ELONEngine result = {};

	//Initialize to stub functions
	result.TeleopCallback = ELONCallbackStub;
	result.TestCallback = ELONCallbackStub;
	result.AutonomousCallback = ELONCallbackStub;
	result.DisabledCallback = ELONCallbackStub;

	void* ELONEngine = dlopen("libELON.so", RTLD_NOW);
	if(ELONEngine){
		//Load real functions
		result.TeleopCallback = (ELONCallback*)dlsym(ELONEngine, "TeleopCallback");
		result.TestCallback = (ELONCallback*)dlsym(ELONEngine, "TestCallback");
		result.DisabledCallback = (ELONCallback*)dlsym(ELONEngine, "AutonomousCallback");
	}

	return result;
}


/*******************************************************************
 * Elevator		                                                   *
 *******************************************************************/

glob Victor elevatorMotor(ELEVATOR_PORT);

MUTEX_ID elevatorMotorLock;

intern void InitializeElevator(){
	elevatorMotorLock = initializeMutexNormal();
}

intern void UpdateElevator(ELONMemory* memory){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ElevatorState* state = scast<ElevatorState*>(&(elonState->elevatorState));
	if(!state->isInitialized){
		state->invertedMotor = 1;
		state->elevatorMagnitude = DEF_SPEED;
		state->isInitialized = True;
	}
	CRITICAL_REGION(elevatorMotorLock);
		elevatorMotor.Set(state->motorValue);
	END_REGION;
}

intern void TerminateElevator(){
	deleteMutex(elevatorMotorLock);
}

/*******************************************************************
 * Chassis		                                                   *
 *******************************************************************/

glob Talon talonFL(CHASSIS_PORT_FL);
glob Talon talonBL(CHASSIS_PORT_BL);
glob Talon talonFR(CHASSIS_PORT_FR);
glob Talon talonBR(CHASSIS_PORT_BR);

MUTEX_ID chassisMotorLock;

intern void InitializeChassis(){
	chassisMotorLock = initializeMutexNormal();
}

intern void UpdateChassis(ELONMemory* memory){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ChassisState* state = scast<ChassisState*>(&(elonState->chassisState));

	if(!state->isInitialized){
		for(U32 i = 0; i < state->nMotors; i++){
			state->invertedMotors[i] = -1;
		}
		state->sensitivity = 0.5f;
		state->chassisMagnitude = DEF_SPEED;
		state->isInitialized = True;
	}
	CRITICAL_REGION(chassisMotorLock);
		talonFL.Set(state->motorValues[0]);
		talonBL.Set(state->motorValues[1]);
		talonFR.Set(state->motorValues[2]);
		talonBR.Set(state->motorValues[3]);
	END_REGION;
}

intern void TerminateChassis(){
	deleteMutex(chassisMotorLock);
}

/*******************************************************************
 * Thread Space                                                    *
 *******************************************************************/

glob MUTEX_ID startedFastThreadLock;
glob MUTEX_ID runningFastThreadLock;
glob MUTEX_ID chassisBufferLock;
glob MUTEX_ID elevatorBufferLock;

glob B32 isFastThreadStarted;
glob B32 isFastThreadRunning;
//TODO: Remove dynamic allocation
typedef std::vector<Action*> ActionVector;
typedef std::vector<Action*>::const_iterator ActionBufferIterator;
typedef std::queue<Action*> ActionQueue;
typedef std::set<Action*> ActionSet;
typedef std::set<Action*>::const_iterator ActionSetIterator;
typedef std::set<Action*>::iterator ActionSetIndex;

glob ActionVector chassisActionBuffer;
glob ActionVector elevatorActionBuffer;
glob ActionQueue chassisActionQueue;
glob ActionSet chassisActionSet;
glob ActionQueue elevatorActionQueue;
glob ActionSet elevatorActionSet;

intern void InitializeThreadSpace(){
	startedFastThreadLock = initializeMutexNormal();
	runningFastThreadLock = initializeMutexNormal();
	chassisBufferLock = initializeMutexNormal();
	elevatorBufferLock = initializeMutexNormal();
	isFastThreadStarted = True;
	isFastThreadRunning = True;
}

intern void TerminateThreadSpace(){
	isFastThreadStarted = False;
	isFastThreadRunning = False;
	takeMutex(elevatorBufferLock);
	deleteMutex(elevatorBufferLock);
	takeMutex(chassisBufferLock);
	deleteMutex(chassisBufferLock);
	takeMutex(runningFastThreadLock);
	deleteMutex(runningFastThreadLock);
	takeMutex(startedFastThreadLock);
	deleteMutex(startedFastThreadLock);
}

intern B32 IsFastThreadStarted(){
	CRITICAL_REGION(startedFastThreadLock);
		return isFastThreadStarted;
	END_REGION;
}

intern B32 IsFastThreadRunning(){
	CRITICAL_REGION(runningFastThreadLock);
		return isFastThreadRunning;
	END_REGION;
}

intern void PauseFastThread(){
	CRITICAL_REGION(runningFastThreadLock);
		isFastThreadRunning = False;
	END_REGION;
}

intern void ResumeFastThread(){
	CRITICAL_REGION(runningFastThreadLock);
		isFastThreadRunning = True;
	END_REGION;
}

intern void StartFastThread(){
	CRITICAL_REGION(startedFastThreadLock);
		isFastThreadStarted = True;
	END_REGION;
}

intern void StopFastThread(){
	CRITICAL_REGION(startedFastThreadLock);
		isFastThreadStarted = False;
	END_REGION;
}

intern void BufferChassisAction(Action* action){
	CRITICAL_REGION(chassisBufferLock);
		if(std::find(chassisActionBuffer.begin(), chassisActionBuffer.end(), action) != chassisActionBuffer.end()){
			return;
		}
		chassisActionBuffer.push_back(action);
	END_REGION;
}

intern void BufferElevatorAction(Action* action){
	CRITICAL_REGION(elevatorBufferLock);
		if(std::find(elevatorActionBuffer.begin(), elevatorActionBuffer.end(), action) != elevatorActionBuffer.end()){
			return;
		}
		elevatorActionBuffer.push_back(action);
	END_REGION;
}

intern void RemoveChassisAction(Action* action){
	if(!action){
		return;
	}

	if(!chassisActionSet.erase(action)){
		return;
	}

	chassisActionQueue.pop();
	action->Removed();
}

intern void RemoveElevatorAction(Action* action){
	if(!action){
		return;
	}

	if(!elevatorActionSet.erase(action)){
		return;
	}

	elevatorActionQueue.pop();
	action->Removed();
}

intern void HandleChassisBufferAdditions(Action* action){
	if(!action){
		return;
	}

	ActionSetIndex found = chassisActionSet.find(action);
	if(found == chassisActionSet.end()){
		chassisActionSet.insert(action);
		chassisActionQueue.push(action);
		action->StartActionFromFastThread();
	}

}

intern void HandleElevatorBufferAdditions(Action* action){
	if(!action){
		return;
	}

	ActionSetIndex found = elevatorActionSet.find(action);
	if(found == elevatorActionSet.end()){
		elevatorActionSet.insert(action);
		elevatorActionQueue.push(action);
		action->StartActionFromFastThread();
	}

}

intern void ExecuteActionQueues(F32 dt){

	Action* chassisAction = chassisActionQueue.front();
	if(!(chassisActionQueue.empty())){
		if(!(chassisAction->Update(dt))){
			RemoveChassisAction(chassisAction);
		}
	}

	Action* elevatorAction = elevatorActionQueue.front();
	if(!(elevatorActionQueue.empty())){
		if(!(elevatorAction->Update(dt))){
			RemoveElevatorAction(elevatorAction);
		}
	}

	CRITICAL_REGION(chassisBufferLock);
		for(ActionBufferIterator i = chassisActionBuffer.begin(); i != chassisActionBuffer.end(); i++){
			HandleChassisBufferAdditions(*i);
		}
		chassisActionBuffer.clear();
	END_REGION;

	CRITICAL_REGION(elevatorBufferLock);
		for(ActionBufferIterator i = elevatorActionBuffer.begin(); i != elevatorActionBuffer.end(); i++){
			HandleElevatorBufferAdditions(*i);
		}
		elevatorActionBuffer.clear();
	END_REGION;
}

intern I32 FastThreadRuntime(U32 targetHz){
#if DISABLE_FAST_THREAD
#else
	F64 targetMSPerFrame = 1000.0 / targetHz;
	F64 startTime = SystemTime();
	F64 lastTime = SystemTime();

	while(IsFastThreadStarted()){
		if(IsFastThreadRunning()){
			//Processing actions
			ExecuteActionQueues(targetMSPerFrame);

			//Updating subsystems


		}
		//Time processing
		F64 workMSElapsed = SystemTime() - lastTime;
		if(workMSElapsed < targetMSPerFrame){
			Wait((targetMSPerFrame - workMSElapsed * 1000.0));
			F64 testMSElapsedForFrame = SystemTime() - lastTime;
			if(testMSElapsedForFrame >= targetMSPerFrame){
				CERR("Fast Thread waited too long.");
			}else{
				do{
					workMSElapsed = SystemTime() - lastTime;
				} while(workMSElapsed <= targetMSPerFrame);
			}
		}else{
			//TODO: MISSED FRAME
			//TODO: Log
		}

		F64 endTime = SystemTime();
		F64 frameTimeMS = endTime - lastTime;
		lastTime = endTime;
		F64 Hz = 1000.0/ frameTimeMS;

		//Frame logging
		//COUT("Last Fast Thread frame time: %.04fms (%.04fHz).", frameTimeMS, Hz);
	}

	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	COUT("[ELON] Total Fast Thread time: %dm%.04fs.", totalMinutes, totalSeconds);
#endif
	return 0;
}

intern I32 CoreThreadRuntime(U32 targetHz, B32_FUNCPTR runnerCallback, ELONCallback* executableCallback, ELON* elon){
#if DISABLE_CORE_THREAD
#else
	F64 targetMSPerFrame = 1000.0 / targetHz;
	F64 lastTime = SystemTime();
	while((elon->*runnerCallback)() && elon->IsEnabled()){
		//Update Input
		UpdateInput(&(elon->elonMemory));

		//Executing user function
		(*executableCallback)(&(elon->elonMemory));

		//Temporary Subsystem updating while fast thread is closed
		UpdateChassis(&(elon->elonMemory));
		UpdateElevator(&(elon->elonMemory));

		//Time processing
		F64 workMSElapsed = SystemTime() - lastTime;
		if(workMSElapsed < targetMSPerFrame){
			Wait((targetMSPerFrame - workMSElapsed * 1000.0));
			F64 testMSElapsedForFrame = SystemTime() - lastTime;
			if(testMSElapsedForFrame >= targetMSPerFrame){
				CERR("Core Thread waited too long.");
			}else{
				do{
					workMSElapsed = SystemTime() - lastTime;
				} while(workMSElapsed <= targetMSPerFrame);
			}
		}else{
			//TODO: MISSED FRAME
			//TODO: Log
			COUT("Missed last Core Thread frame.");
		}

		F64 endTime = SystemTime();
		F64 frameTimeMS = endTime - lastTime;
		lastTime = endTime;
		F64 Hz = 1000.0/ frameTimeMS;

		//Frame logging
		//COUT("Last Core Thread frame time: %.04fms (%.04fHz).", frameTimeMS, Hz);
	}
#endif
	return 0;
}

/*******************************************************************
 * Input					                                       *
 *******************************************************************/

#define _LX 0
#define _LY 1
#define _LT 2
#define _RT 3
#define _RX 4
#define _RY 5
#define NUM_BUTTONS 10

glob DriverStation* ds; //Driverstation where gamepads are connected

intern void UpdateInput(ELONMemory* memory){
	ELONState* state = scast<ELONState*>(memory->permanentStorage);

	for(U32 i = 0; i < NUM_GAMEPADS; i++){
		state->gamepads[i].buttons |= state->gamepads[i].buttons << 16;
		state->gamepads[i].buttons = ds->GetStickButtons(i);

		//Circular deadzone processing of left joystick
		F32 lx = ds->GetStickAxis(i, _LX);
		F32 ly = ds->GetStickAxis(i, _LY);
		F32 lmgntd2 = lx*lx+ly*ly;

		if(lmgntd2 < deadzone2){
			state->gamepads[i].lx = 0.0f;
			state->gamepads[i].ly = 0.0f;
		}else{
			F32 lmgntd = sqrtf(lmgntd2);
			F32 nlxFactor = lx / lmgntd;
			F32 nlyFactor = ly / lmgntd;
			if(lmgntd > 1.0f){
				lmgntd = 1.0f;
			}
			lmgntd -= deadzone;
			lx = nlxFactor * lmgntd;
			ly = nlyFactor * lmgntd;
			if(state->gamepads[i].inputType == InputType::LINEAR){
				state->gamepads[i].lx = lx / (1 - deadzone * nlxFactor);
				state->gamepads[i].ly = ly / (1 - deadzone * nlyFactor);
			}else if(state->gamepads[i].inputType == InputType::QUADRATIC){
				state->gamepads[i].lx = Sq(lx / (1 - deadzone * nlxFactor * Sgn(lx))) * Sgn(lx);
				state->gamepads[i].ly = Sq(ly / (1 - deadzone * nlyFactor * Sgn(ly))) * Sgn(ly);
			}else if(state->gamepads[i].inputType == InputType::QUARTIC){
				state->gamepads[i].lx = Qu(lx / (1 - deadzone * nlxFactor * Sgn(lx))) * Sgn(lx);
				state->gamepads[i].ly = Qu(ly / (1 - deadzone * nlyFactor * Sgn(ly))) * Sgn(ly);
			}
		}

		//Circular deadzone processing of right joystick
		F32 rx = ds->GetStickAxis(i, _RX);
		F32 ry = ds->GetStickAxis(i, _RY);
		F32 rmgntd2 = rx*rx+ry*ry;

		if(rmgntd2 < deadzone2){
			state->gamepads[i].rx = 0.0f;
			state->gamepads[i].ry = 0.0f;
		}else{
			F32 rmgntd = sqrtf(rmgntd2);
			F32 nrxFactor = rx / rmgntd;
			F32 nryFactor = ry / rmgntd;
			if(rmgntd > 1.0f){
				rmgntd = 1.0f;
			}
			rmgntd -= deadzone;
			rx = nrxFactor * rmgntd;
			ry = nryFactor * rmgntd;
			if(state->gamepads[i].inputType == InputType::LINEAR){
				state->gamepads[i].rx = rx / (1 - deadzone * nrxFactor * Sgn(rx));
				state->gamepads[i].ry = ry / (1 - deadzone * nryFactor * Sgn(ry));
			}else if(state->gamepads[i].inputType == InputType::QUADRATIC){
				state->gamepads[i].rx = Sq(rx / (1 - deadzone * nrxFactor * Sgn(rx))) * Sgn(rx);
				state->gamepads[i].ry = Sq(ry / (1 - deadzone * nryFactor * Sgn(ry))) * Sgn(ry);
			}else if(state->gamepads[i].inputType == InputType::QUARTIC){
				state->gamepads[i].rx = Qu(rx / (1 - deadzone * nrxFactor * Sgn(rx))) * Sgn(rx);
				state->gamepads[i].ry = Qu(ry / (1 - deadzone * nryFactor * Sgn(ry))) * Sgn(ry);
			}
		}

		//Linear deadzone processing of left trigger
		F32 lt = ds->GetStickAxis(i, _LT);
		if(lt < triggerDeadzone){
			state->gamepads[i].lt = 0.0f;
		}else{
			if(state->gamepads[i].inputType == InputType::LINEAR){
				state->gamepads[i].lt = (lt - triggerDeadzone * Sgn(lt))/(1.0f - triggerDeadzone);
			}else if(state->gamepads[i].inputType == InputType::QUADRATIC){
				state->gamepads[i].lt = Sq((lt - triggerDeadzone * Sgn(lt))/(1.0f - triggerDeadzone));
			}else if(state->gamepads[i].inputType == InputType::QUARTIC){
				state->gamepads[i].lt = Qu((lt - triggerDeadzone * Sgn(lt))/(1.0f - triggerDeadzone));
			}
		}

		//Linear deadzone processing of right trigger
		F32 rt = ds->GetStickAxis(i, _RT);
		if(rt < triggerDeadzone){
			state->gamepads[i].rt = 0.0f;
		}else{
			if(state->gamepads[i].inputType == InputType::LINEAR){
				state->gamepads[i].rt = (rt - triggerDeadzone * Sgn(rt))/(1.0f - triggerDeadzone);
			}else if(state->gamepads[i].inputType == InputType::QUADRATIC){
				state->gamepads[i].rt = Sq((rt - triggerDeadzone * Sgn(rt))/(1.0f - triggerDeadzone));
			}else if(state->gamepads[i].inputType == InputType::QUARTIC){
				state->gamepads[i].rt = Qu((rt - triggerDeadzone * Sgn(rt))/(1.0f - triggerDeadzone));
			}
		}


		//Stash POV (D-PAD) angles in degrees
		state->gamepads[i].dpad = ds->GetStickPOV(i, 0);
	}
}

/*******************************************************************
 * ELON Class				                                       *
 *******************************************************************/

ELON* elon;

ELON::ELON(){
	COUT("Initializing");

	//ELONEngine startup
	engine = scast<ELONEngine>(malloc(sizeof(ELONEngine)));

#if 0
	void* baseAddress = rcast<void*>(U32((GiB(4) - 1) - ((ELON_TOTAL_STORAGE_SIZE / getpagesize()) + 1) * getpagesize()));
#else
	void* baseAddress = NULL;
#endif

	//Memory startup
	elonMemory = {};
	elonMemory.permanentStorageSize = ELON_PERMANENT_STORAGE_SIZE;
	elonMemory.transientStorageSize = ELON_TRANSIENT_STORAGE_SIZE;



	totalElonMemoryBlock = mmap(baseAddress, ELON_TOTAL_STORAGE_SIZE, PROT_READ | PROT_WRITE,
								MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, 0, 0);

	if(totalElonMemoryBlock){
		COUT("Total ELON memory successfully allocated with size of %u Bytes", ELON_TOTAL_STORAGE_SIZE);
	}else{
		CERR("Total ELON memory allocation failed with size of %u Bytes", ELON_TOTAL_STORAGE_SIZE);
	}

	elonMemory.permanentStorage = totalElonMemoryBlock;
	elonMemory.transientStorage = ((U8*)elonMemory.permanentStorage + elonMemory.permanentStorageSize);

	//Thread startup
	InitializeThreadSpace();
	fastThread = new Task("FastThread", (FUNCPTR)(&FastThreadRuntime), 100, KiB(5));
#if DISABLE_FAST_THREAD
#else
	fastThread->Start(FAST_THREAD_HZ);
#endif
	StartFastThread();
	PauseFastThread();
}

void ELON::RobotInit(){
	//System startup

	//Chassis initialization
	InitializeChassis();

	//Elevator initialization
	InitializeElevator();
}

void ELON::Autonomous(){
	F64 startTime = SystemTime();
	ResumeFastThread();

	CoreThreadRuntime(CORE_THREAD_HZ, (B32_FUNCPTR)(&ELON::IsAutonomous),
					 &(engine->AutonomousCallback), this);

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
					 engine->TeleopCallback, this);

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
					 engine->TestCallback, this);

	PauseFastThread();
	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	COUT("Total Test time: %dm%.04fs.", totalMinutes, totalSeconds);
}

void ELON::Disabled(){
	F64 startTime = SystemTime();

	CoreThreadRuntime(CORE_THREAD_HZ, (B32_FUNCPTR)(&ELON::IsTest),
					 engine->DisabledCallback, this);

	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	COUT("Total Disabled time: %dm%.04fs.", totalMinutes, totalSeconds);
}

ELON::~ELON(){
	//System shutdown
	TerminateElevator();
	TerminateChassis();

	//Thread shutdown
	StopFastThread();
	fastThread->Stop();
	TerminateThreadSpace();
	delete fastThread;

	//Memory shutdown
	munmap(totalElonMemoryBlock, ELON_TOTAL_STORAGE_SIZE);

	free(engine);

	//Logging shutdown
	TerminateLogging();
}

/*******************************************************************
 * Main function                                                   *
 *******************************************************************/

I32 main() {
	//Logging startup
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
