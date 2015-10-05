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
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <unistd.h>
#include "stdio.h"
#include "stdarg.h"
#include "../inc/EHL.h"
#include "../inc/ELONEngine.h"



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
 * Util						                                       *
 *******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get FGPA Time in milliseconds
 */
SYSTEM_TIME_CALLBACK(SystemTime){
	return GetFPGATime() / 1000.0;
}

/**
 * Clamps value of a between b and c
 */
F32_CALLBACK_F32_F32_F32(Clamp){
	if(a < b) return b;
	else if(a > c) return c;
	return a;
}

/**
 * Returns the greater value of a and b
 */
F32_CALLBACK_F32_F32(Max){
	if(b > a) return b;
	return a;
}

/**
 * Returns the value of x squared
 */
F32_CALLBACK_F32(Sq){
	return x * x;
}

/**
 * Returns the value of x cubed
 */
F32_CALLBACK_F32(Cu){
	return x * x * x;
}


/**
 * Returns the value of x quarted
 */
F32_CALLBACK_F32(Qu){
	return x * x * x * x;
}

/**
 * Return sign of x
 */
I32_CALLBACK_F32(Sgn){
	return (0 < x) - (x < 0);
}

/**
 * Normalizes x of interval [-1,1] to interval [0,1]
 */
F32_CALLBACK_F32(NormalizeAlpha){
	return 0.5f * x + 0.5f;
}

/**
 * Linearly interpolates between a and b in the scale of value c of interval [0,1]
 */
F32_CALLBACK_F32_F32_F32(Lerp){
	return (1.0f - c) * a + c * b;
}

/**
 * Calculates cosine interpolation between a and b in the scale of value c of interval [0,1]
 */
F32_CALLBACK_F32_F32_F32(Coserp){
	F32 alpha = (1.0f - cosf(c * PI)) * 0.5f;
	return (1.0f - alpha) * a + alpha * b;
}

/**
 * Calculates the system magnitude by interpolation between a, b, c in the scale of d of interval [-1,1]
 */
F32_CALLBACK_F32_F32_F32_F32(SystemMagnitudeInterpolation){
	if(d < 0.0f){
		d++;
		return Coserp(a, b, d);
	}else{
		return Coserp(b, c, d);
	}
}

/**
 * Calculates principle angle in degrees from interval [-INFINITY,INFINITY] to interval [0,360]
 */
F32_CALLBACK_F32(PrincipalAngleDeg){
	return x - (I32)(x/360) * 360; //TODO: Test
}

/**
 * Calculates principle angle in radians from interval [-INFINITY,INFINITY] to interval [0,TAU]
 */
F32_CALLBACK_F32(PrincipalAngleRad){
	return x - (I32)(x/TAU) * TAU; //TODO: Test
}

/**
 * Calculate minimum distance angle in degrees from interval [-INFINITY,INFINITY] to interval [-180,180]
 */
F32_CALLBACK_F32(MinDistAngleDeg){
	return (x - (I32)(x/360) * 360) - 180.0f; //TODO: Test
}

/**
 * Calculate minimum distance angle in radians from interval [-INFINITY,INFINITY] to interval [-PI,PI]
 */
F32_CALLBACK_F32(MinDistAngleRad){
	return (x - (I32)(x/TAU) * TAU) - PI; //TODO: Test
}

/**
 * Calculates the shortest angular distance in degrees between two angles of interval [-INFINITY,INFINITY] to [-180,180]
 */
F32_CALLBACK_F32_F32(AngularDistDeg){
	return MinDistAngleDeg(b - a);
}

/**
 * Calculates the shortest angular distance in radians between two angles of interval [-INFINITY,INFINITY] to [-PI,PI]
 */
F32_CALLBACK_F32_F32(AngularDistRad){
	return MinDistAngleRad(b - a);
}

/**
 * Return 10 raised to the exponent of 10
 */
U64_CALLBACK_U32(Pow10){
	U64 result = 1;
	for(U32 i = 0; i < x; i++){
		result *= 10;
	}
	return result;
}

/**
 * Converts decimal integer to binary (buggy)
 */
U64_CALLBACK_U32(DecToBin){
	U64 bin = 0;
	for(int i = 0; x != 0; i++){
		bin += Pow10(i) * (x % 2);
		x /= 2;
	}
	return bin;
}

inline U32 KiB(U32 x){
	return x * 1024;
}

inline U32 MiB(U32 x){
	return KiB(x) * 1024;
}

inline U32 GiB(U32 x){
	return MiB(x) * 1024;
}

inline U32 TiB(U32 x){
	return GiB(x) * 1024;
}

/**
 * Represents file
 */
struct File{
	void* data;
	U32 size;
};

/**
 * Reads entire file and returns File struct
 */
File ReadEntireFile(std::string filename){
	File result = {};
	struct stat statBuffer;

	I32 fileDescriptor = open(filename.c_str(), O_RDONLY);
	if(fileDescriptor != -1){
		fstat(fileDescriptor, &statBuffer);
		result.size = statBuffer.st_size;
		if(result.size){
			Cout("File opened: \"%s\"; Size: %lu", filename.c_str(), result.size);
			result.data = mmap(NULL, result.size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fileDescriptor, 0);
			if(result.data != MAP_FAILED){
				U32 bytesRead = read(fileDescriptor, result.data, result.size);
				if(bytesRead != result.size){
					Cerr("File allocation failed: File: \"%s\"; Size %lu", filename.c_str(), result.size);
					munmap(result.data, result.size);
					result.data = NULL;
					result.size = 0;
				}
			}else{
				Cerr("File request allocation failed: File \"%s\"; Size %lu", filename.c_str(), result.size);
			}
		}else{
			Cerr("File request failed with size of 0; File: \"%s\"", filename.c_str());
		}
	}
	close(fileDescriptor);
	return result;
}

/**
 * Writes memory to file of given filename, creates file if doesn't exist
 */
B32 WriteEntireFile(std::string filename, U32 memorySize, void* memory){
	B32 result = false;

	I32 fileDescriptor = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fileDescriptor != -1){
		U32 bytesWritten = write(fileDescriptor, memory, memorySize);
		result = bytesWritten == memorySize;
		close(fileDescriptor);
	}else{
		I32 err = errno;
		Cerr("Could not create file: \"%s\"", filename.c_str());
		Cerr("Errno: %d", err);
	}

	return result;
}

/**
 * Copies one file to another
 * TODO: optimize or find better solution
 */
B32 CopyFile(std::string src, std::string dest){
	File srcFile = ReadEntireFile(src);
	return WriteEntireFile(dest, srcFile.size, srcFile.data);
}

/**
 * Returns last time a file was written to
 */
I64 GetLastWriteTime(std::string filename){
	I64 result = 0;
	struct stat statBuffer;
	if(stat(filename.c_str(), &statBuffer)){
		Cerr("Could not get stat of file: %s", filename.c_str());
		result = 0;
	}else{
		result = statBuffer.st_mtim.tv_sec;
	}
	return result;
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
	I64 lastWriteTime;
	ELONCallback* InitTeleop;
	ELONCallback* TeleopCallback;
	ELONCallback* InitTest;
	ELONCallback* TestCallback;
	ELONCallback* InitAutonomous;
	ELONCallback* AutonomousCallback;
	ELONCallback* InitDisabled;
	ELONCallback* DisabledCallback;
	B32 isValid;
};

ELON_CALLBACK(ELONCallbackStub){

}

/**
 * Loads ELONEngine
 */
ELONEngine LoadELONEngine(std::string filename){
	ELONEngine result = {};

	result.lastWriteTime = GetLastWriteTime(filename);
	if(CopyFile(filename, ELONEngineTempBinary)){

		result.ELONEngine = dlopen(ELONEngineTempBinary, RTLD_NOW);
		if(result.ELONEngine){
			//Load real functions
			result.InitTeleop = (ELONCallback*)dlsym(result.ELONEngine, "InitTeleop");
			result.TeleopCallback = (ELONCallback*)dlsym(result.ELONEngine, "TeleopCallback");
			result.InitTest = (ELONCallback*)dlsym(result.ELONEngine, "InitTest");
			result.TestCallback = (ELONCallback*)dlsym(result.ELONEngine, "TestCallback");
			result.InitAutonomous = (ELONCallback*)dlsym(result.ELONEngine, "InitAutonomous");
			result.AutonomousCallback = (ELONCallback*)dlsym(result.ELONEngine, "AutonomousCallback");
			result.InitDisabled = (ELONCallback*)dlsym(result.ELONEngine, "InitDisabled");
			result.DisabledCallback = (ELONCallback*)dlsym(result.ELONEngine, "DisabledCallback");

			result.isValid = (result.InitTeleop && result.TeleopCallback && result.InitTest && result.TestCallback
							 && result.InitAutonomous && result.AutonomousCallback && result.InitDisabled
							 && result.DisabledCallback);
		}else{
			Cerr("Invalid ELONEngine binary: \"%s\"", ELONEngineBinary);
		}
	}else{
		Cerr("Could not copy \"%s\" into \"%s\"", ELONEngineBinary, ELONEngineTempBinary);
	}

	if(!(result.isValid)){
		//Initialize to stub functions
		result.InitTeleop = ELONCallbackStub;
		result.TeleopCallback = ELONCallbackStub;
		result.InitTest = ELONCallbackStub;
		result.TestCallback = ELONCallbackStub;
		result.InitAutonomous = ELONCallbackStub;
		result.AutonomousCallback = ELONCallbackStub;
		result.InitDisabled = ELONCallbackStub;
		result.DisabledCallback = ELONCallbackStub;
	}else{
		Cout("ELONEngine successfully loaded!");
	}

	return result;
}

/**
 * Unloads ELONEngine
 */
void UnloadELONEngine(ELONEngine* engine){
	if(engine->ELONEngine){
		dlclose(engine->ELONEngine);
	}

	engine->isValid = False;
	engine->InitTeleop = ELONCallbackStub;
	engine->TeleopCallback = ELONCallbackStub;
	engine->InitTest = ELONCallbackStub;
	engine->TestCallback = ELONCallbackStub;
	engine->InitAutonomous = ELONCallbackStub;
	engine->AutonomousCallback = ELONCallbackStub;
	engine->InitDisabled = ELONCallbackStub;
	engine->DisabledCallback = ELONCallbackStub;
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
	Cout("Elevator Initialized");
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
	Cout("Chassis Initialized");
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
				Cerr("Fast Thread waited too long.");
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
	Cout("[ELON] Total Fast Thread time: %dm%.04fs.", totalMinutes, totalSeconds);
#endif
	return 0;
}

#endif

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

void UpdateInput(DriverStation* ds, ELONMemory* memory){
	ELONState* state = scast<ELONState*>(memory->permanentStorage);

	for(U32 i = 0; i < NUM_GAMEPADS; i++){
		if(!state){
			Cerr("Gamepad is null: gamepads[%d] Total gamepads: %d", i, NUM_GAMEPADS);
		}
		state->gamepads[i].buttons |= state->gamepads[i].buttons << 16;
		state->gamepads[i].buttons = ds->GetStickButtons(i);

		//Circular deadzone processing of left joystick
		F32 lx = ds->GetStickAxis(i, _LX);
		F32 ly = ds->GetStickAxis(i, _LY);
		F32 lmgntd2 = lx*lx+ly*ly;


		if(lmgntd2 < DEADZONE_SQ){
			state->gamepads[i].lx = 0.0f;
			state->gamepads[i].ly = 0.0f;
		}else{
			F32 lmgntd = sqrtf(lmgntd2);
			F32 nlxFactor = lx / lmgntd;
			F32 nlyFactor = ly / lmgntd;
			if(lmgntd > 1.0f){
				lmgntd = 1.0f;
			}
			lmgntd -= DEADZONE;
			lx = nlxFactor * lmgntd;
			ly = nlyFactor * lmgntd;
			if(state->gamepads[i].inputType == InputType::LINEAR){
				state->gamepads[i].lx = lx / (1 - DEADZONE * nlxFactor);
				state->gamepads[i].ly = ly / (1 - DEADZONE * nlyFactor);
			}else if(state->gamepads[i].inputType == InputType::QUADRATIC){
				state->gamepads[i].lx = memory->Sq(lx / (1 - DEADZONE * nlxFactor * memory->Sgn(lx))) * memory->Sgn(lx);
				state->gamepads[i].ly = memory->Sq(ly / (1 - DEADZONE * nlyFactor * memory->Sgn(ly))) * memory->Sgn(ly);
			}else if(state->gamepads[i].inputType == InputType::QUARTIC){
				state->gamepads[i].lx = memory->Qu(lx / (1 - DEADZONE * nlxFactor * memory->Sgn(lx))) * memory->Sgn(lx);
				state->gamepads[i].ly = memory->Qu(ly / (1 - DEADZONE * nlyFactor * memory->Sgn(ly))) * memory->Sgn(ly);
			}
		}

		//Circular deadzone processing of right joystick
		F32 rx = ds->GetStickAxis(i, _RX);
		F32 ry = ds->GetStickAxis(i, _RY);
		F32 rmgntd2 = rx*rx+ry*ry;

		if(rmgntd2 < DEADZONE_SQ){
			state->gamepads[i].rx = 0.0f;
			state->gamepads[i].ry = 0.0f;
		}else{
			F32 rmgntd = sqrtf(rmgntd2);
			F32 nrxFactor = rx / rmgntd;
			F32 nryFactor = ry / rmgntd;
			if(rmgntd > 1.0f){
				rmgntd = 1.0f;
			}
			rmgntd -= DEADZONE;
			rx = nrxFactor * rmgntd;
			ry = nryFactor * rmgntd;
			if(state->gamepads[i].inputType == InputType::LINEAR){
				state->gamepads[i].rx = rx / (1 - DEADZONE * nrxFactor * memory->Sgn(rx));
				state->gamepads[i].ry = ry / (1 - DEADZONE * nryFactor * memory->Sgn(ry));
			}else if(state->gamepads[i].inputType == InputType::QUADRATIC){
				state->gamepads[i].rx = memory->Sq(rx / (1 - DEADZONE * nrxFactor * memory->Sgn(rx))) * memory->Sgn(rx);
				state->gamepads[i].ry = memory->Sq(ry / (1 - DEADZONE * nryFactor * memory->Sgn(ry))) * memory->Sgn(ry);
			}else if(state->gamepads[i].inputType == InputType::QUARTIC){
				state->gamepads[i].rx = memory->Qu(rx / (1 - DEADZONE * nrxFactor * memory->Sgn(rx))) * memory->Sgn(rx);
				state->gamepads[i].ry = memory->Qu(ry / (1 - DEADZONE * nryFactor * memory->Sgn(ry))) * memory->Sgn(ry);
			}
		}

		//Linear deadzone processing of left trigger
		F32 lt = ds->GetStickAxis(i, _LT);
		if(lt < TRIGGER_DEADZONE){
			state->gamepads[i].lt = 0.0f;
		}else{
			if(state->gamepads[i].inputType == InputType::LINEAR){
				state->gamepads[i].lt = (lt - TRIGGER_DEADZONE * memory->Sgn(lt))/(1.0f - TRIGGER_DEADZONE);
			}else if(state->gamepads[i].inputType == InputType::QUADRATIC){
				state->gamepads[i].lt = memory->Sq((lt - TRIGGER_DEADZONE * memory->Sgn(lt))/(1.0f - TRIGGER_DEADZONE));
			}else if(state->gamepads[i].inputType == InputType::QUARTIC){
				state->gamepads[i].lt = memory->Qu((lt - TRIGGER_DEADZONE * memory->Sgn(lt))/(1.0f - TRIGGER_DEADZONE));
			}
		}

		//Linear deadzone processing of right trigger
		F32 rt = ds->GetStickAxis(i, _RT);
		if(rt < TRIGGER_DEADZONE){
			state->gamepads[i].rt = 0.0f;
		}else{
			if(state->gamepads[i].inputType == InputType::LINEAR){
				state->gamepads[i].rt = (rt - TRIGGER_DEADZONE * memory->Sgn(rt))/(1.0f - TRIGGER_DEADZONE);
			}else if(state->gamepads[i].inputType == InputType::QUADRATIC){
				state->gamepads[i].rt = memory->Sq((rt - TRIGGER_DEADZONE * memory->Sgn(rt))/(1.0f - TRIGGER_DEADZONE));
			}else if(state->gamepads[i].inputType == InputType::QUARTIC){
				state->gamepads[i].rt = memory->Qu((rt - TRIGGER_DEADZONE * memory->Sgn(rt))/(1.0f - TRIGGER_DEADZONE));
			}
		}


		//Stash POV (D-PAD) angles in degrees
		state->gamepads[i].dpad = ds->GetStickPOV(i, 0);
	}
}

/*******************************************************************
 * ELON Class				                                       *
 *******************************************************************/

U32 ELON_PERMANENT_STORAGE_SIZE = (MiB(16));
U32 ELON_TRANSIENT_STORAGE_SIZE = (MiB(16));
U32 ELON_TOTAL_STORAGE_SIZE  = (ELON_PERMANENT_STORAGE_SIZE + ELON_TRANSIENT_STORAGE_SIZE);

ELON::ELON(){

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

}

void ELON::RobotMain(){
	Cout("Starting ELON Hardware Layer Core Loop");

	LiveWindow* lw = LiveWindow::GetInstance();
	DriverStation* ds = DriverStation::GetInstance();
	lw->SetEnabled(True);

	Cout("Initializing");

	//System startup

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
								MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if(totalElonMemoryBlock != MAP_FAILED){
		Cout("Total ELON memory successfully allocated with size of %lu Bytes", ELON_TOTAL_STORAGE_SIZE);
	}else{
		I32 err = errno;
		Cerr("Total ELON memory allocation failed with size of %lu Bytes", ELON_TOTAL_STORAGE_SIZE);
		Cerr("Errno: %d", err);
	}

	elonMemory.permanentStorage = totalElonMemoryBlock;
	elonMemory.transientStorage = ((U8*)elonMemory.permanentStorage + elonMemory.permanentStorageSize);

	//Chassis initialization
	InitializeChassis();

	//Elevator initialization
	InitializeElevator();

	//ELONEngine startup
	ELONEngine engine = LoadELONEngine(ELONEngineBinary);
	U32 loadCounter = 0;

	F64 targetMSPerFrame = 1000.0 / CORE_THREAD_HZ;
	F64 startTime = SystemTime();
	F64 lastTime = SystemTime();
	for(;;){
		//Reload ELONEngine
		U32 newELONEngineWriteTime = GetLastWriteTime(ELONEngineBinary);
		//Cout("%d", newELONEngineWriteTime);
		if(newELONEngineWriteTime != engine.lastWriteTime){
			UnloadELONEngine(&engine);
			engine = LoadELONEngine(ELONEngineBinary);
			loadCounter = 0;
		}

		//Update Input
		UpdateInput(ds, &elonMemory);

		//Executing user function based on robot state
		if(IsAutonomous() && IsEnabled()){
			if(!autonomousInit){
				lw->SetEnabled(False);
				ds->InAutonomous(True);
				ds->InOperatorControl(False);
				ds->InTest(False);
				ds->InDisabled(False);
				ELON::Autonomous();
				engine.InitAutonomous(&elonMemory);
				autonomousInit = True;
				teleopInit = False;
				testInit = False;
				disabledInit = False;
			}
			//Autonomous Iterative Dytor
			engine.AutonomousCallback(&elonMemory);

		}else if(IsOperatorControl() && IsEnabled()){
			if(!autonomousInit){
				lw->SetEnabled(False);
				ds->InAutonomous(False);
				ds->InOperatorControl(True);
				ds->InTest(False);
				ds->InDisabled(False);
				ELON::OperatorControl();
				engine.InitTeleop(&elonMemory);
				autonomousInit = False;
				teleopInit = True;
				testInit = False;
				disabledInit = False;
			}
			//Teleop Iterative Dytor
			engine.TeleopCallback(&elonMemory);

		}else if(IsTest() && IsEnabled()){
			if(!autonomousInit){
				lw->SetEnabled(True);
				ds->InAutonomous(False);
				ds->InOperatorControl(False);
				ds->InTest(True);
				ds->InDisabled(False);
				ELON::Test();
				engine.InitTest(&elonMemory);
				autonomousInit = False;
				teleopInit = False;
				testInit = True;
				disabledInit = False;
			}
			//Test Iterative Dytor
			engine.TestCallback(&elonMemory);

		}else if(IsDisabled()){
			if(!autonomousInit){
				lw->SetEnabled(False);
				ds->InAutonomous(False);
				ds->InOperatorControl(False);
				ds->InTest(False);
				ds->InDisabled(True);
				ELON::Disabled();
				engine.InitDisabled(&elonMemory);
				autonomousInit = False;
				teleopInit = False;
				testInit = False;
				disabledInit = True;
			}
			//Disabled Iterative Dytor
			engine.DisabledCallback(&elonMemory);

		}

		//Temporary Subsystem updating while fast thread is closed
		UpdateChassis(&elonMemory);
		UpdateElevator(&elonMemory);

		//Time processing
		F64 workMSElapsed = SystemTime() - lastTime;
		if(workMSElapsed < targetMSPerFrame){
			Wait((targetMSPerFrame - workMSElapsed * 1000.0));
			F64 testMSElapsedForFrame = SystemTime() - lastTime;
			if(testMSElapsedForFrame >= targetMSPerFrame){
				Cerr("Core Thread Runtime waited too long.");
			}else{
				do{
					workMSElapsed = SystemTime() - lastTime;
				} while(workMSElapsed <= targetMSPerFrame);
			}
		}else{
			//TODO: MISSED FRAME
			//TODO: Log
			Cout("Missed last Core Thread Runtime frame.");
		}

		F64 endTime = SystemTime();
		F64 frameTimeMS = endTime - lastTime;
		lastTime = endTime;
		F64 Hz = 1000.0/ frameTimeMS;

		//Frame logging
		//COUT("Last Core Thread frame time: %.04fms (%.04fHz).", frameTimeMS, Hz);
	}

	//System shutdown
	TerminateElevator();
	TerminateChassis();

	//Thread shutdown
	//StopFastThread();
	//fastThread->Stop();
	//TerminateThreadSpace();
	delete fastThread;

	//Memory shutdown
	munmap(totalElonMemoryBlock, ELON_TOTAL_STORAGE_SIZE);

	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	Cout("Total Core Thread Running time: %dm%.04fs.", totalMinutes, totalSeconds);
}

void ELON::Autonomous(){

}

void ELON::OperatorControl(){

}

void ELON::Test(){

}

void ELON::Disabled(){

}

ELON::~ELON(){
	//Logging shutdown
	TerminateLogging();
}

/*******************************************************************
 * Main function                                                   *
 *******************************************************************/

I32 main() {
	//Logging startup
	InitializeLogging();
	Cout("Logging Initialized!");

	if (!HALInitialize()){
		Cerr("HAL could not be initialized.");
		return -1;
	}
	HALReport(HALUsageReporting::kResourceType_Language, HALUsageReporting::kLanguage_CPlusPlus);

	ELON* elon = new ELON();
	RobotBase::robotSetup(elon);
	Cout("Does this run?");
	return 0;
}
