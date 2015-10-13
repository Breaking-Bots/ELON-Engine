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
#include "EHL.h"
#include "ELONEngine.h"



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
	return x * x;
}

F32_CALLBACK_F32(Cu){
	return x * x * x;
}


F32_CALLBACK_F32(Qu){
	return x * x * x * x;
}

I32_CALLBACK_F32(Sgn){
	return (0 < x) - (x < 0);
}

F32_CALLBACK_F32(NormalizeAlpha){
	return 0.5f * x + 0.5f;
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
	return x - (I32)(x/360) * 360; //TODO: Test
}

F32_CALLBACK_F32(PrincipalAngleRad){
	return x - (I32)(x/TAU) * TAU; //TODO: Test
}

F32_CALLBACK_F32(MinDistAngleDeg){
	return (x - (I32)(x/360) * 360) - 180.0f; //TODO: Test
}

F32_CALLBACK_F32(MinDistAngleRad){
	return (x - (I32)(x/TAU) * TAU) - PI; //TODO: Test
}

F32_CALLBACK_F32_F32(AngularDistDeg){
	return MinDistAngleDeg(b - a);
}

F32_CALLBACK_F32_F32(AngularDistRad){
	return MinDistAngleRad(b - a);
}

U64_CALLBACK_U32(Pow10){
	U64 result = 1;
	for(U32 i = 0; i < x; i++){
		result *= 10;
	}
	return result;
}

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

U32 GetFileSize(std::string filename, HANDLE fd, B32 useFD, B32 ignoreFailure){
	struct stat statBuffer;
	U32 result = 0;
	if(!useFD){
		fd = open(filename.c_str(), O_RDONLY);
	}
	if(fd != -1){
		fstat(fd, &statBuffer);
		result = statBuffer.st_size;
		if(!useFD){
			close(fd);
		}
	}
	return result;
}

File ReadEntireFile(std::string filename, B32 ignoreFailure){
	File result = {};
	struct stat statBuffer;

	HANDLE fileDescriptor = open(filename.c_str(), O_RDONLY);
	if(fileDescriptor != -1){
		result.size = GetFileSize(filename, fileDescriptor, True, ignoreFailure);
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
				if(!ignoreFailure){
					Cerr("File request allocation failed: File \"%s\"; Size %lu", filename.c_str(), result.size);
				}
			}
		}else{
			if(!ignoreFailure){
				Cerr("File request failed with size of 0; File: \"%s\"", filename.c_str());
			}
		}
		close(fileDescriptor);
	}
	return result;
}

B32 WriteEntireFile(std::string filename, U32 memorySize, void* memory, B32 ignoreFailure){
	B32 result = false;

	HANDLE fileDescriptor = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fileDescriptor != -1){
		U32 bytesWritten = write(fileDescriptor, memory, memorySize);
		result = bytesWritten == memorySize;
		close(fileDescriptor);
	}else{
		if(!ignoreFailure){
			I32 err = errno;
			Cerr("Could not create file: \"%s\"", filename.c_str());
			Cerr("Errno: %d", err);
		}
	}

	return result;
}

B32 CopyFile(std::string src, std::string dest, B32 ignoreFailure){
	File srcFile = ReadEntireFile(src, ignoreFailure);
	return WriteEntireFile(dest, srcFile.size, srcFile.data, ignoreFailure);
}

I64 GetLastWriteTime(std::string filename, B32 ignoreFailure){
	I64 result = 0;
	struct stat statBuffer;
	if(!stat(filename.c_str(), &statBuffer)){
		result = statBuffer.st_mtim.tv_sec;
	}else{
		if(!ignoreFailure){
			Cerr("Could not get stat of file: %s", filename.c_str());
		}
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

ELON_CALLBACK(ELONCallbackStub){

}


ELONEngine LoadELONEngine(std::string filename){
	ELONEngine result = {};

	result.lastWriteTime = GetLastWriteTime(filename);
	if(CopyFile(filename, ELONEngineTempBinary)){

		result.module = dlopen(ELONEngineTempBinary, RTLD_NOW);
		if(result.module){
			//Load real functions
			result.InitTeleop = (ELONCallback*)dlsym(result.module, "InitTeleop");
			result.TeleopCallback = (ELONCallback*)dlsym(result.module, "TeleopCallback");
			result.InitTest = (ELONCallback*)dlsym(result.module, "InitTest");
			result.TestCallback = (ELONCallback*)dlsym(result.module, "TestCallback");
			result.InitAutonomous = (ELONCallback*)dlsym(result.module, "InitAutonomous");
			result.AutonomousCallback = (ELONCallback*)dlsym(result.module, "AutonomousCallback");
			result.InitDisabled = (ELONCallback*)dlsym(result.module, "InitDisabled");
			result.DisabledCallback = (ELONCallback*)dlsym(result.module, "DisabledCallback");

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


void UnloadELONEngine(ELONEngine* engine){
	if(engine->module){
		dlclose(engine->module);
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

intern void ProcessDigitalButton(U32 buttonBitSet, ButtonState* oldState, U32 buttonBit, ButtonState* newState){
	newState->endedDown = ((buttonBitSet & buttonBit) == buttonBit);
	newState->halfTransitionCount = (oldState->endedDown != newState->endedDown) ? 1 : 0;
}

intern void ProcessStickInput(F32* _x, F32* _y, InputType inputType){
	F32 x = *_x;
	F32 y = *_y;
	F32 mgntdSquared = x*x+y*y;

	if(mgntdSquared < DEADZONE_SQ){
		x = 0.0f;
		y = 0.0f;
	}else{
		F32 mgntd = sqrtf(mgntdSquared);
		F32 nlxFactor = x / mgntd;
		F32 nlyFactor = y / mgntd;
		if(mgntd > 1.0f){
			mgntd = 1.0f;
		}
		mgntd -= DEADZONE;
		x = nlxFactor * mgntd;
		y = nlyFactor * mgntd;
		if(inputType == InputType::LINEAR){
			x = x / (1 - DEADZONE * nlxFactor);
			y = y / (1 - DEADZONE * nlyFactor);
		}else if(inputType == InputType::QUADRATIC){
			x = Sq(x / (1 - DEADZONE * nlxFactor * Sgn(x))) * Sgn(x);
			y = Sq(y / (1 - DEADZONE * nlyFactor * Sgn(y))) * Sgn(y);
		}else if(inputType == InputType::QUARTIC){
			x = Qu(x / (1 - DEADZONE * nlxFactor * Sgn(x))) * Sgn(x);
			y = Qu(y / (1 - DEADZONE * nlyFactor * Sgn(y))) * Sgn(y);
		}
	}

	*_x = x;
	*_y = y;
}

intern void ProcessTriggerInput(F32* _t, InputType inputType){
	F32 t = *_t;
	if(t < TRIGGER_DEADZONE){
		t = 0.0f;
	}else{
		if(inputType == InputType::LINEAR){
			t = (t - TRIGGER_DEADZONE * Sgn(t))/(1.0f - TRIGGER_DEADZONE);
		}else if(inputType == InputType::QUADRATIC){
			t = Sq((t - TRIGGER_DEADZONE * Sgn(t))/(1.0f - TRIGGER_DEADZONE));
		}else if(inputType == InputType::QUARTIC){
			t = Qu((t - TRIGGER_DEADZONE * Sgn(t))/(1.0f - TRIGGER_DEADZONE));
		}
	}

	*_t = t;
}

void UpdateInput(DriverStation* ds, Input* newInput, Input* oldInput){


	for(U32 i = 0; i < NUM_GAMEPADS; i++){

		Gamepad* newGamepad = GetGamepad(newInput, i);
		Gamepad* oldGamepad = GetGamepad(oldInput, i);

		//Processing buttons
		U32 buttonBitSet = ds->GetStickButtons(i);

		for(U32 j = 0; j < NUM_BUTTONS; j++){
			ProcessDigitalButton(buttonBitSet, &oldGamepad->buttons[i], j, &newGamepad->buttons[i]);
		}

		//Processing DPAD values
		I32 dpad = ds->GetStickPOV(i, 0);

		if(dpad != -1){
			ProcessDigitalButton((dpad >= 315 || dpad <= 45)? 1 : 0, &oldGamepad->up, 1, &newGamepad->up);
			ProcessDigitalButton((dpad >= 45 && dpad <= 135)? 1 : 0, &oldGamepad->right, 1, &newGamepad->right);
			ProcessDigitalButton((dpad >= 135 && dpad <= 225)? 1 : 0, &oldGamepad->down, 1, &newGamepad->down);
			ProcessDigitalButton((dpad >= 225 && dpad <= 315)? 1 : 0, &oldGamepad->left, 1, &newGamepad->left);
		}

		//Circular deadzone processing of left joystick
		F32 lx = ds->GetStickAxis(i, _LX);
		F32 ly = ds->GetStickAxis(i, _LY);

		ProcessStickInput(&lx, &ly, newGamepad->inputType);
		newGamepad->lx = lx;
		newGamepad->ly = ly;

		//Circular deadzone processing of right joystick
		F32 rx = ds->GetStickAxis(i, _RX);
		F32 ry = ds->GetStickAxis(i, _RY);

		ProcessStickInput(&rx, &ry, newGamepad->inputType);
		newGamepad->rx = rx;
		newGamepad->ry = ry;

		//Linear deadzone processing of left trigger
		F32 lt = ds->GetStickAxis(i, _LT);
		ProcessTriggerInput(&lt, newGamepad->inputType);
		newGamepad->lt = lt;

		//Linear deadzone processing of right trigger
		F32 rt = ds->GetStickAxis(i, _RT);
		ProcessTriggerInput(&rt, newGamepad->inputType);
		newGamepad->rt = rt;
	}
}

intern void BeginTeleopInputRecording(EHLState* state, U32 lastTeleopRecordingIndex){
	state->lastTeleopRecordingIndex = lastTeleopRecordingIndex;

	time_t t = time(NULL);
	struct tm* time = localtime(&t);
	char str_time[100];
	strftime(str_time, sizeof(str_time) - 1, "_%Y%m%d%H%M%s.eid", time);
	char filename[256];
	filename = ELONTeleopRecordingDataFile;
	strcat(filename, str_time);

	state->lastTeleopRecordingHandle = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(state->totalSize >= GiB(2)){
		Cerr("Too much memory allocated to write to input data file");
	}

	write(state->recordingHandle, state->totalELONMemoryBlock, state->totalSize);
}

intern void EndTeleopInputRecording(EHLState* state){
	close(state->lastTeleopRecordingHandle);
	state->lastTeleopRecordingIndex = 0;
}

//TODO: Lazily write block to memory and use memory copy instead?
intern void BeginInputRecording(EHLState* state, U32 inputRecordingIndex){
	state->inputRecordingIndex = inputRecordingIndex;
	state->recordingHandle = open(ELONInputRecordingDataFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);

	if(state->totalSize >= GiB(2)){
		Cerr("Too much memory allocated to write to input data file");
	}

	write(state->recordingHandle, state->totalELONMemoryBlock, state->totalSize);
}

intern void EndInputRecording(EHLState* state){
	close(state->recordingHandle);
	state->inputRecordingIndex = 0;
}

intern void BeginInputPlayBack(EHLState* state, U32 inputRecordingIndex){
	state->inputPlayBackIndex = inputRecordingIndex;
	state->playBackHandle = open(ELONInputRecordingDataFile, O_RDONLY);

	read(state->recordingHandle, state->totalELONMemoryBlock, state->totalSize);
}

intern void EndInputPlayBack(EHLState* state){
	close(state->playBackHandle);
	state->inputPlayBackIndex = 0;
}

intern void RecordInput(EHLState* state, Input* input){
	write(state->recordingHandle, input, sizeof(*input));
}

intern void PlayBackInput(EHLState* state, Input* input){
	if(read(state->playBackHandle, input, sizeof(*input)) < 1){
		U32 playBackIndex = state->inputPlayBackIndex;
		EndInputPlayBack(state);
		BeginInputPlayBack(state, playBackIndex);
		read(state->playBackHandle, input, sizeof(*input));
	}
}

intern void ProcessEHLInputProtocols(EHLState* state, Input* input){
	for(U32 i = 0; i < NUM_GAMEPADS; i++){
		if(input->gamepads[i].a.endedDown && !input->gamepads[i].a.halfTransitionCount){
			if(state->inputRecordingIndex == 0){
				BeginInputRecording(state, 1);
			}else{
				EndInputRecording(state);
				BeginInputPlayBack(state, 1);
			}
		}
	}
}


/*******************************************************************
 * ELON Class				                                       *
 *******************************************************************/

U32 ELON_PERMANENT_STORAGE_SIZE = (MiB(1));
U32 ELON_TRANSIENT_STORAGE_SIZE = (MiB(1));
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

	LiveWindow* lw = LiveWindow::GetInstance();
	DriverStation* ds = DriverStation::GetInstance();
	lw->SetEnabled(True);

	Cout("Initializing");

	//System startup

	//Memory startup
	EHLState ehlState = {};
	ELONMemory elonMemory;

#if 1
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

	ehlState.totalSize = ELON_TOTAL_STORAGE_SIZE;
	ehlState.totalELONMemoryBlock = mmap(baseAddress, ehlState.totalSize, PROT_READ | PROT_WRITE,
								MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if(ehlState.totalELONMemoryBlock != MAP_FAILED){
		Cout("Total ELON memory successfully allocated with size of %lu Bytes", ehlState.totalSize);
	}else{
		I32 err = errno;
		Cerr("Total ELON memory allocation failed with size of %lu Bytes", ehlState.totalSize);
		Cerr("Errno: %d", err);
	}

	elonMemory.permanentStorage = ehlState.totalELONMemoryBlock;
	elonMemory.transientStorage = ((U8*)elonMemory.permanentStorage + elonMemory.permanentStorageSize);

	//Input initialization
	Input inputs[2];
	inputs[0] = {};
	inputs[1] = {};
	Input* newInput = &inputs[0];
	Input* oldInput = &inputs[1];

	//Chassis initialization
	InitializeChassis();

	//Elevator initialization
	InitializeElevator();

	//ELONEngine startup
	ELONEngine engine = LoadELONEngine(ELONEngineBinary);
	U32 loadCounter = 0;
	B32 autonomousInit = False;
	B32 teleopInit = False;
	B32 testInit = False;
	B32 disabledInit = False;

	F64 targetMSPerFrame = 1000.0 / CORE_THREAD_HZ;
	F64 startTime = SystemTime();
	F64 lastTime = SystemTime();
	Cout("Starting ELON Hardware Layer Core Loop");
	for(;;){
		//Reload ELONEngine
		U32 newELONEngineWriteTime = GetLastWriteTime(ELONEngineBinary);
		//Cout("%d", newELONEngineWriteTime);
		U32 libELONFileSize = GetFileSize(ELONEngineBinary);
		U32 libELON_tempFileSize = GetFileSize(ELONEngineTempBinary);
		if((newELONEngineWriteTime != engine.lastWriteTime) || (libELONFileSize != libELON_tempFileSize)){
			UnloadELONEngine(&engine);
			engine = LoadELONEngine(ELONEngineBinary);
			loadCounter = 0;
		}

		//Update Input
		UpdateInput(ds, newInput, oldInput);
		ProcessEHLInputProtocols(&ehlState, newInput);

		//Executing user function based on robot state
		if(IsAutonomous() && IsEnabled()){
			if(!autonomousInit){
				if(ehlState.lastTeleopRecordingIndex){
					EndTeleopInputRecording(&ehlState);
				}

				lw->SetEnabled(False);
				ds->InAutonomous(True);
				ds->InOperatorControl(False);
				ds->InTest(False);
				ds->InDisabled(False);
				ELON::Autonomous();
				engine.InitAutonomous(&elonMemory, newInput);
				autonomousInit = True;
				teleopInit = False;
				testInit = False;
				disabledInit = False;
			}
			//Autonomous Iterative Dytor
			engine.AutonomousCallback(&elonMemory, newInput);

		}else if(IsOperatorControl() && IsEnabled()){
			if(!autonomousInit){
				if(ehlState.lastTeleopRecordingIndex){
					EndTeleopInputRecording(&ehlState);
				}

				BeginTeleopInputRecording(&ehlState, 1);
				lw->SetEnabled(False);
				ds->InAutonomous(False);
				ds->InOperatorControl(True);
				ds->InTest(False);
				ds->InDisabled(False);
				ELON::OperatorControl();
				engine.InitTeleop(&elonMemory, newInput);
				autonomousInit = False;
				teleopInit = True;
				testInit = False;
				disabledInit = False;
			}
			//Teleop Iterative Dytor
			engine.TeleopCallback(&elonMemory, newInput);

		}else if(IsTest() && IsEnabled()){

			//Input Recording
			if(ehlState.inputRecordingIndex){
				RecordInput(&ehlState, newInput);
			}

			//Input PlayBack
			if(ehlState.inputPlayBackIndex){
				PlayBackInput(&ehlState, newInput);
			}

			if(!autonomousInit){
				if(ehlState.lastTeleopRecordingIndex){
					EndTeleopInputRecording(&ehlState);
				}

				lw->SetEnabled(True);
				ds->InAutonomous(False);
				ds->InOperatorControl(False);
				ds->InTest(True);
				ds->InDisabled(False);
				ELON::Test();
				engine.InitTest(&elonMemory, newInput);
				autonomousInit = False;
				teleopInit = False;
				testInit = True;
				disabledInit = False;
			}
			//Test Iterative Dytor
			engine.TestCallback(&elonMemory, newInput);

		}else if(IsDisabled()){
			if(!autonomousInit){
				if(ehlState.lastTeleopRecordingIndex){
					EndTeleopInputRecording(&ehlState);
				}

				lw->SetEnabled(False);
				ds->InAutonomous(False);
				ds->InOperatorControl(False);
				ds->InTest(False);
				ds->InDisabled(True);
				ELON::Disabled();
				engine.InitDisabled(&elonMemory, newInput);
				autonomousInit = False;
				teleopInit = False;
				testInit = False;
				disabledInit = True;
			}
			//Disabled Iterative Dytor
			engine.DisabledCallback(&elonMemory, newInput);

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
	munmap(ehlState.totalELONMemoryBlock, ELON_TOTAL_STORAGE_SIZE);

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
