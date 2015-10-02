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
#include "stdio.h"
#include "stdarg.h"
#include "EHL.h"
#include "ELONEngine.h"


/*******************************************************************
 * Util						                                       *
 *******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

SYSTEM_TIME_CALLBACK(SystemTime){
	return GetFPGATime() / 1000.0;
}

F32_CALLBACK_F32_F32_F32(Clamp){
	if(a < b) return b;
	else if(a > c) return c;
	return a;
}


F32_CALLBACK_F32_F32(Max){
	if(b > a) return b;
	return a;
}


F32_CALLBACK_F32(Sq){
	return a * a;
}

F32_CALLBACK_F32(Cu){
	return a * a * a;
}


F32_CALLBACK_F32(Qu){
	return a * a * a * a;
}


I32_CALLBACK_F32(Sgn){
	return (0 < a) - (a < 0);
}


F32_CALLBACK_F32(NormalizeAlpha){
	return 0.5f * a + 0.5f;
}


F32_CALLBACK_F32_F32_F32(Lerp){
	return (1.0f - c) * a + c * b;
}


F32_CALLBACK_F32_F32_F32(Coserp){
	F32 alpha = (1.0f - cosf(c * PI)) * 0.5f;
	return (1.0f - alpha) * a + alpha * b;
}


F32_CALLBACK_F32_F32_F32_F32(SystemMagnitudeInterpolation){
	if(d < 0.0f){
		d++;
		return Coserp(a, b, d);
	}else{
		return Coserp(b, c, d);
	}
}

F32_CALLBACK_F32(PrincipalAngleDeg){
	return a - (I32)(a/360) * 360; //TODO: Test
}

F32_CALLBACK_F32(PrincipalAngleRad){
	return a - (I32)(a/TAU) * TAU; //TODO: Test
}

F32_CALLBACK_F32(MinDistAngleDeg){
	return (a - (I32)(a/360) * 360) - 180.0f; //TODO: Test
}

F32_CALLBACK_F32(MinDistAngleRad){
	return (a - (I32)(a/TAU) * TAU) - PI; //TODO: Test
}

F32_CALLBACK_F32_F32(AngularDistDeg){
	return MinDistAngleDeg(b - a);
}

F32_CALLBACK_F32_F32(AngularDistRad){
	return MinDistAngleRad(b - a);
}

U64_CALLBACK_U32(Pow10){
	U64 result = 1;
	for(U32 i = 0; i < a; i++){
		result *= 10;
	}
	return result;
}

U64_CALLBACK_U32(DecToBin){
	U64 bin = 0;
	for(int i = 0; a != 0; i++){
		bin += Pow10(i) * (a % 2);
		a /= 2;
	}
	return bin;
}

#ifdef __cplusplus
}
#endif

/*******************************************************************
 * Logging					                                       *
 *******************************************************************/

MUTEX_ID loggingLock;

#ifdef __cplusplus
extern "C" {
#endif

void InitializeLogging(){
	loggingLock = initializeMutexNormal();
}

LOGGING_CALLBACK(Cout){
	CRITICAL_REGION(loggingLock);
		const char* formattedCStr = format.c_str();
		char* fmt = new char[strlen(formattedCStr) + 10];
		strcpy(fmt, "[ELON] ");
		strcpy(fmt + 7, formattedCStr);
		strcpy(fmt + strlen(fmt), "\n");
		va_list args;
		va_start(args, format);
		I32 result = vprintf(fmt, args);
		va_end(args);
		return result;
	END_REGION;
}

LOGGING_CALLBACK(Cerr){
	CRITICAL_REGION(loggingLock);
		const char* formattedCStr = format.c_str();
		char* fmt = new char[strlen(formattedCStr) + 11];
		strcpy(fmt, "[ERROR] ");
		strcpy(fmt + 8, formattedCStr);
		strcpy(fmt + strlen(fmt), "\n");
		va_list args;
		va_start(args, format);
		I32 result = vprintf(fmt, args);
		va_end(args);
		return result;
	END_REGION;
}

void TerminateLogging(){
	takeMutex(loggingLock);
	deleteMutex(loggingLock);
}

#ifdef __cplusplus
}
#endif

/*******************************************************************
 * ELON Engine Management                                          *
 *******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

struct ELONEngine{
	MODULE ELONEngine;
	ELONCallback* TeleopCallback;
	ELONCallback* TestCallback;
	ELONCallback* AutonomousCallback;
	ELONCallback* DisabledCallback;
	B32 isValid;
};

ELON_CALLBACK(ELONCallbackStub){

}

ELONEngine LoadELONEngine(){
	ELONEngine result = {};

	result.ELONEngine = dlopen("/home/lvuser/libELON.so", RTLD_NOW);
	if(result.ELONEngine){
		//Load real functions
		result.TeleopCallback = (ELONCallback*)dlsym(result.ELONEngine, "TeleopCallback");
		result.TestCallback = (ELONCallback*)dlsym(result.ELONEngine, "TestCallback");
		result.AutonomousCallback = (ELONCallback*)dlsym(result.ELONEngine, "AutonomousCallback");
		result.DisabledCallback = (ELONCallback*)dlsym(result.ELONEngine, "DisabledCallback");

		result.isValid = (result.TeleopCallback && result.TestCallback && result.AutonomousCallback
						 && result.DisabledCallback);
	}

	if(!(result.isValid)){
		//Initialize to stub functions
		result.TeleopCallback = ELONCallbackStub;
		result.TestCallback = ELONCallbackStub;
		result.AutonomousCallback = ELONCallbackStub;
		result.DisabledCallback = ELONCallbackStub;
	}

	return result;
}

#ifdef __cplusplus
}
#endif

/*******************************************************************
 * Elevator		                                                   *
 *******************************************************************/
#if 1
Victor* elevatorMotor;

MUTEX_ID elevatorMotorLock;

void InitializeElevator(){
	elevatorMotorLock = initializeMutexNormal();
	elevatorMotor = new Victor(ELEVATOR_PORT);
}

void UpdateElevator(ELONMemory* memory){
	ELONState* elonState = scast<ELONState*>(memory->permanentStorage);
	ElevatorState* state = scast<ElevatorState*>(&(elonState->elevatorState));
	if(!state->isInitialized){
		state->invertedMotor = 1;
		state->elevatorMagnitude = DEF_SPEED;
		state->isInitialized = True;
	}
	CRITICAL_REGION(elevatorMotorLock);
		//elevatorMotor.Set(state->motorValue);
	END_REGION;
}

void TerminateElevator(){
	delete elevatorMotor;
	deleteMutex(elevatorMotorLock);
}
#endif
/*******************************************************************
 * Chassis		                                                   *
 *******************************************************************/
#if 1
Talon* motors[CHASSIS_NUM_MOTORS];

MUTEX_ID chassisMotorLock;

void InitializeChassis(){
	chassisMotorLock = initializeMutexNormal();
	motors[0] = new Talon(CHASSIS_PORT_FL);
	motors[1] = new Talon(CHASSIS_PORT_BL);
	motors[2] = new Talon(CHASSIS_PORT_FR);
	motors[3] = new Talon(CHASSIS_PORT_BR);
}

void UpdateChassis(ELONMemory* memory){
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
		for(U32 i = 0; i < CHASSIS_NUM_MOTORS; i++){
			motors[i]->Set(state->motorValues[i]);
		}
	END_REGION;
}

void TerminateChassis(){
	for(U32 i = 0; i < CHASSIS_NUM_MOTORS; i++){
		delete motors[i];
	}
	deleteMutex(chassisMotorLock);
}
#endif
/*******************************************************************
 * Thread Space                                                    *
 *******************************************************************/
#if 0

MUTEX_ID startedFastThreadLock;
MUTEX_ID runningFastThreadLock;
MUTEX_ID chassisBufferLock;
MUTEX_ID elevatorBufferLock;

B32 isFastThreadStarted;
B32 isFastThreadRunning;
//TODO: Remove dynamic allocation
typedef std::vector<Action*> ActionVector;
typedef std::vector<Action*>::const_iterator ActionBufferIterator;
typedef std::queue<Action*> ActionQueue;
typedef std::set<Action*> ActionSet;
typedef std::set<Action*>::const_iterator ActionSetIterator;
typedef std::set<Action*>::iterator ActionSetIndex;

ActionVector chassisActionBuffer;
ActionVector elevatorActionBuffer;
ActionQueue chassisActionQueue;
ActionSet chassisActionSet;
ActionQueue elevatorActionQueue;
ActionSet elevatorActionSet;

void InitializeThreadSpace(){
	startedFastThreadLock = initializeMutexNormal();
	runningFastThreadLock = initializeMutexNormal();
	chassisBufferLock = initializeMutexNormal();
	elevatorBufferLock = initializeMutexNormal();
	isFastThreadStarted = True;
	isFastThreadRunning = True;
}

void TerminateThreadSpace(){
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

B32 IsFastThreadStarted(){
	CRITICAL_REGION(startedFastThreadLock);
		return isFastThreadStarted;
	END_REGION;
}

B32 IsFastThreadRunning(){
	CRITICAL_REGION(runningFastThreadLock);
		return isFastThreadRunning;
	END_REGION;
}

void PauseFastThread(){
	CRITICAL_REGION(runningFastThreadLock);
		isFastThreadRunning = False;
	END_REGION;
}

void ResumeFastThread(){
	CRITICAL_REGION(runningFastThreadLock);
		isFastThreadRunning = True;
	END_REGION;
}

void StartFastThread(){
	CRITICAL_REGION(startedFastThreadLock);
		isFastThreadStarted = True;
	END_REGION;
}

void StopFastThread(){
	CRITICAL_REGION(startedFastThreadLock);
		isFastThreadStarted = False;
	END_REGION;
}

void BufferChassisAction(Action* action){
	CRITICAL_REGION(chassisBufferLock);
		if(std::find(chassisActionBuffer.begin(), chassisActionBuffer.end(), action) != chassisActionBuffer.end()){
			return;
		}
		chassisActionBuffer.push_back(action);
	END_REGION;
}

void BufferElevatorAction(Action* action){
	CRITICAL_REGION(elevatorBufferLock);
		if(std::find(elevatorActionBuffer.begin(), elevatorActionBuffer.end(), action) != elevatorActionBuffer.end()){
			return;
		}
		elevatorActionBuffer.push_back(action);
	END_REGION;
}

void RemoveChassisAction(Action* action){
	if(!action){
		return;
	}

	if(!chassisActionSet.erase(action)){
		return;
	}

	chassisActionQueue.pop();
	//action->Removed();
}

void RemoveElevatorAction(Action* action){
	if(!action){
		return;
	}

	if(!elevatorActionSet.erase(action)){
		return;
	}

	elevatorActionQueue.pop();
	//action->Removed();
}

void HandleChassisBufferAdditions(Action* action){
	if(!action){
		return;
	}

	ActionSetIndex found = chassisActionSet.find(action);
	if(found == chassisActionSet.end()){
		chassisActionSet.insert(action);
		chassisActionQueue.push(action);
		//action->StartActionFromFastThread();
	}

}

void HandleElevatorBufferAdditions(Action* action){
	if(!action){
		return;
	}

	ActionSetIndex found = elevatorActionSet.find(action);
	if(found == elevatorActionSet.end()){
		elevatorActionSet.insert(action);
		elevatorActionQueue.push(action);
		//action->StartActionFromFastThread();
	}

}

void ExecuteActionQueues(F32 dt){

	Action* chassisAction = chassisActionQueue.front();
	if(!(chassisActionQueue.empty())){
		//if(!(chassisAction->Update(dt))){
		//	RemoveChassisAction(chassisAction);
		//}
	}

	Action* elevatorAction = elevatorActionQueue.front();
	if(!(elevatorActionQueue.empty())){
		//if(!(elevatorAction->Update(dt))){
		//	RemoveElevatorAction(elevatorAction);
		//}
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

I32 FastThreadRuntime(U32 targetHz){
#if DISABLE_FAST_THREAD
#else
	F64 targetMSPerFrame = 1000.0 / targetHz;
	F64 startTime = elon->elonMemory.SystemTime();
	F64 lastTime = elon->elonMemory.SystemTime();

	while(IsFastThreadStarted()){
		if(IsFastThreadRunning()){
			//Processing actions
			ExecuteActionQueues(targetMSPerFrame);

			//Updating subsystems


		}
		//Time processing
		F64 workMSElapsed = elon->elonMemory.SystemTime() - lastTime;
		if(workMSElapsed < targetMSPerFrame){
			Wait((targetMSPerFrame - workMSElapsed * 1000.0));
			F64 testMSElapsedForFrame = elon->elonMemory.SystemTime() - lastTime;
			if(testMSElapsedForFrame >= targetMSPerFrame){
				elon->elonMemory.Cerr("Fast Thread waited too long.");
			}else{
				do{
					workMSElapsed = elon->elonMemory.SystemTime() - lastTime;
				} while(workMSElapsed <= targetMSPerFrame);
			}
		}else{
			//TODO: MISSED FRAME
			//TODO: Log
		}

		F64 endTime = elon->elonMemory.SystemTime();
		F64 frameTimeMS = endTime - lastTime;
		lastTime = endTime;
		F64 Hz = 1000.0/ frameTimeMS;

		//Frame logging
		//COUT("Last Fast Thread frame time: %.04fms (%.04fHz).", frameTimeMS, Hz);
	}

	F64 totalTimeElapsedSeconds = (elon->elonMemory.SystemTime() - startTime) * 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	elon->elonMemory.Cout("[ELON] Total Fast Thread time: %dm%.04fs.", totalMinutes, totalSeconds);
#endif
	return 0;
}

#endif

I32 CoreThreadRuntime(U32 targetHz, B32_FUNCPTR runnerCallback, ELONCallback* executableCallback, ELON* elon){
#if DISABLE_CORE_THREAD
#else
	F64 targetMSPerFrame = 1000.0 / targetHz;
	F64 lastTime = elon->elonMemory->SystemTime();
	while((elon->*runnerCallback)() && elon->IsEnabled()){
		//Update Input
		UpdateInput(elon->elonMemory);

		//Executing user function
		(*executableCallback)(elon->elonMemory);

		//Temporary Subsystem updating while fast thread is closed
		UpdateChassis(elon->elonMemory);
		UpdateElevator(elon->elonMemory);

		//Time processing
		F64 workMSElapsed = elon->elonMemory->SystemTime() - lastTime;
		if(workMSElapsed < targetMSPerFrame){
			Wait((targetMSPerFrame - workMSElapsed * 1000.0));
			F64 testMSElapsedForFrame = elon->elonMemory->SystemTime() - lastTime;
			if(testMSElapsedForFrame >= targetMSPerFrame){
				elon->elonMemory->Cerr("Core Thread waited too long.");
			}else{
				do{
					workMSElapsed = elon->elonMemory->SystemTime() - lastTime;
				} while(workMSElapsed <= targetMSPerFrame);
			}
		}else{
			//TODO: MISSED FRAME
			//TODO: Log
			elon->elonMemory->Cout("Missed last Core Thread frame.");
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

DriverStation* ds; //Driverstation where gamepads are connected

void UpdateInput(ELONMemory* memory){
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
				state->gamepads[i].lx = memory->Sq(lx / (1 - deadzone * nlxFactor * memory->Sgn(lx))) * memory->Sgn(lx);
				state->gamepads[i].ly = memory->Sq(ly / (1 - deadzone * nlyFactor * memory->Sgn(ly))) * memory->Sgn(ly);
			}else if(state->gamepads[i].inputType == InputType::QUARTIC){
				state->gamepads[i].lx = memory->Qu(lx / (1 - deadzone * nlxFactor * memory->Sgn(lx))) * memory->Sgn(lx);
				state->gamepads[i].ly = memory->Qu(ly / (1 - deadzone * nlyFactor * memory->Sgn(ly))) * memory->Sgn(ly);
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
				state->gamepads[i].rx = rx / (1 - deadzone * nrxFactor * memory->Sgn(rx));
				state->gamepads[i].ry = ry / (1 - deadzone * nryFactor * memory->Sgn(ry));
			}else if(state->gamepads[i].inputType == InputType::QUADRATIC){
				state->gamepads[i].rx = memory->Sq(rx / (1 - deadzone * nrxFactor * memory->Sgn(rx))) * memory->Sgn(rx);
				state->gamepads[i].ry = memory->Sq(ry / (1 - deadzone * nryFactor * memory->Sgn(ry))) * memory->Sgn(ry);
			}else if(state->gamepads[i].inputType == InputType::QUARTIC){
				state->gamepads[i].rx = memory->Qu(rx / (1 - deadzone * nrxFactor * memory->Sgn(rx))) * memory->Sgn(rx);
				state->gamepads[i].ry = memory->Qu(ry / (1 - deadzone * nryFactor * memory->Sgn(ry))) * memory->Sgn(ry);
			}
		}

		//Linear deadzone processing of left trigger
		F32 lt = ds->GetStickAxis(i, _LT);
		if(lt < triggerDeadzone){
			state->gamepads[i].lt = 0.0f;
		}else{
			if(state->gamepads[i].inputType == InputType::LINEAR){
				state->gamepads[i].lt = (lt - triggerDeadzone * memory->Sgn(lt))/(1.0f - triggerDeadzone);
			}else if(state->gamepads[i].inputType == InputType::QUADRATIC){
				state->gamepads[i].lt = memory->Sq((lt - triggerDeadzone * memory->Sgn(lt))/(1.0f - triggerDeadzone));
			}else if(state->gamepads[i].inputType == InputType::QUARTIC){
				state->gamepads[i].lt = memory->Qu((lt - triggerDeadzone * memory->Sgn(lt))/(1.0f - triggerDeadzone));
			}
		}

		//Linear deadzone processing of right trigger
		F32 rt = ds->GetStickAxis(i, _RT);
		if(rt < triggerDeadzone){
			state->gamepads[i].rt = 0.0f;
		}else{
			if(state->gamepads[i].inputType == InputType::LINEAR){
				state->gamepads[i].rt = (rt - triggerDeadzone * memory->Sgn(rt))/(1.0f - triggerDeadzone);
			}else if(state->gamepads[i].inputType == InputType::QUADRATIC){
				state->gamepads[i].rt = memory->Sq((rt - triggerDeadzone * memory->Sgn(rt))/(1.0f - triggerDeadzone));
			}else if(state->gamepads[i].inputType == InputType::QUARTIC){
				state->gamepads[i].rt = memory->Qu((rt - triggerDeadzone * memory->Sgn(rt))/(1.0f - triggerDeadzone));
			}
		}


		//Stash POV (D-PAD) angles in degrees
		state->gamepads[i].dpad = ds->GetStickPOV(i, 0);
	}
}

/*******************************************************************
 * ELON Class				                                       *
 *******************************************************************/

ELONEngine engine;

ELON::ELON(ELONMemory* memory): elonMemory(memory){
	Cout("Initializing");

	//Thread startup
	//InitializeThreadSpace();
	//fastThread = new Task("FastThread", (FUNCPTR)(&FastThreadRuntime), 100, KiB(5));
#if DISABLE_FAST_THREAD
#else
	fastThread->Start(FAST_THREAD_HZ);
#endif
	//StartFastThread();
	//PauseFastThread();
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
	//ResumeFastThread();

	CoreThreadRuntime(CORE_THREAD_HZ, (B32_FUNCPTR)(&ELON::IsAutonomous),
					 engine.AutonomousCallback, this);

	//PauseFastThread();
	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	Cout("Total Autonomous time: %dm%.04fs.", totalMinutes, totalSeconds);
}

void ELON::OperatorControl(){
	F64 startTime = SystemTime();
	//ResumeFastThread();

	CoreThreadRuntime(CORE_THREAD_HZ, (B32_FUNCPTR)(&ELON::IsOperatorControl),
					 engine.TeleopCallback, this);

	//PauseFastThread();
	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) / 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	Cout("Total Teleoperator time: %dm%.04fs.", totalMinutes, totalSeconds);
}

void ELON::Test(){
	F64 startTime = SystemTime();
	//ResumeFastThread();

	CoreThreadRuntime(CORE_THREAD_HZ, (B32_FUNCPTR)(&ELON::IsTest),
					 engine.TestCallback, this);

	//PauseFastThread();
	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	Cout("Total Test time: %dm%.04fs.", totalMinutes, totalSeconds);
}

void ELON::Disabled(){
	F64 startTime = SystemTime();

	CoreThreadRuntime(CORE_THREAD_HZ, (B32_FUNCPTR)(&ELON::IsTest),
					 engine.DisabledCallback, this);

	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	Cout("Total Disabled time: %dm%.04fs.", totalMinutes, totalSeconds);
}

ELON::~ELON(){
	//System shutdown
	TerminateElevator();
	TerminateChassis();

	//Thread shutdown
	//StopFastThread();
	fastThread->Stop();
	//TerminateThreadSpace();
	delete fastThread;

	//Memory shutdown
	//munmap(totalElonMemoryBlock, ELON_TOTAL_STORAGE_SIZE);

	//Logging shutdown
	TerminateLogging();
}

/*******************************************************************
 * Main function                                                   *
 *******************************************************************/

U64 KiB(U64 sizeBytes){
	return sizeBytes * 1024LL;
}

U64 MiB(U64 sizeBytes){
	return KiB(sizeBytes) * 1024LL;
}

U64 GiB(U64 sizeBytes){
	return MiB(sizeBytes) * 1024LL;
}

U64 TiB(U64 sizeBytes){
	return TiB(sizeBytes) * 1024LL;
}

U32 ELON_PERMANENT_STORAGE_SIZE = (MiB(16));
U32 ELON_TRANSIENT_STORAGE_SIZE = (MiB(16));
U32 ELON_TOTAL_STORAGE_SIZE = (ELON_PERMANENT_STORAGE_SIZE + ELON_TRANSIENT_STORAGE_SIZE);

I32 main() {
	//Logging startup
	InitializeLogging();
	Cout("Logging Initialized!");


	//Memory startup
	void* totalElonMemoryBlock = NULL;
	ELONMemory elonMemory;

#if 0
	void* baseAddress = rcast<void*>(U32((GiB(4) - 1) - ((ELON_TOTAL_STORAGE_SIZE / getpagesize()) + 1) * getpagesize()));
#else
	void* baseAddress = NULL;
#endif

	elonMemory = {};
	elonMemory.permanentStorageSize = ELON_PERMANENT_STORAGE_SIZE;
	elonMemory.transientStorageSize = ELON_TRANSIENT_STORAGE_SIZE;
	elonMemory.Cout = Cout;
	elonMemory.Cerr = Cerr;
	elonMemory.SystemTime = SystemTime;
	elonMemory.Clamp = Clamp;
	elonMemory.Max = Max;
	elonMemory.Sq = Sq;
	elonMemory.Cu = Cu;
	elonMemory.Qu = Qu;
	elonMemory.NormalizeAlpha = NormalizeAlpha;
	elonMemory.Lerp = Lerp;
	elonMemory.Coserp = Coserp;
	elonMemory.SystemMagnitudeInterpolation = SystemMagnitudeInterpolation;
	elonMemory.PrincipalAngleDeg = PrincipalAngleDeg;
	elonMemory.PrincipalAngleRad = PrincipalAngleRad;
	elonMemory.MinDistAngleDeg = MinDistAngleDeg;
	elonMemory.MinDistAngleRad = MinDistAngleRad;
	elonMemory.AngularDistDeg = AngularDistDeg;
	elonMemory.AngularDistRad = AngularDistRad;
	elonMemory.Pow10 = Pow10;
	elonMemory.DecToBin = DecToBin;

	totalElonMemoryBlock = mmap(baseAddress, ELON_TOTAL_STORAGE_SIZE, PROT_READ | PROT_WRITE,
								MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, 0, 0);

	if(totalElonMemoryBlock){
		Cout("Total ELON memory successfully allocated with size of %d Bytes", ELON_TOTAL_STORAGE_SIZE);
	}else{
		Cerr("Total ELON memory allocation failed with size of %d Bytes", ELON_TOTAL_STORAGE_SIZE);
	}

	elonMemory.permanentStorage = totalElonMemoryBlock;
	elonMemory.transientStorage = ((U8*)elonMemory.permanentStorage + elonMemory.permanentStorageSize);
	//ELONEngine startup
	engine = LoadELONEngine();

	if (!HALInitialize()){
		Cerr("HAL could not be initialized.");
		return -1;
	}
	HALReport(HALUsageReporting::kResourceType_Language, HALUsageReporting::kLanguage_CPlusPlus);

	ELON* elon = new ELON(&elonMemory);
	RobotBase::robotSetup(elon);
	Cout("Does this run?");
	return 0;
}
