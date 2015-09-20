/*
 * ThreadSpace.cpp
 *
 *  Created on: Sep 18, 2015
 *      Author: Zaeem
 */

#include "WPILib.h"
#include "ThreadSpace.h"
#include "Util.h"
#include "ELON.h"
#include "Action.h"
#include <set>
#include <vector>
#include <queue>
#include <algorithm>

MUTEX_ID startedFastThreadLock;
MUTEX_ID runningFastThreadLock;
MUTEX_ID chassisBufferLock;
MUTEX_ID elevatorBufferLock;

B32 isFastThreadStarted;
B32 isFastThreadRunning;
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
	isFastThreadStarted = TRUE;
	isFastThreadRunning = TRUE;
}

void TerminateThreadSpace(){
	isFastThreadStarted = FALSE;
	isFastThreadRunning = FALSE;
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
		isFastThreadRunning = FALSE;
	END_REGION;
}

void ResumeFastThread(){
	CRITICAL_REGION(runningFastThreadLock);
		isFastThreadRunning = TRUE;
	END_REGION;
}

void StartFastThread(){
	CRITICAL_REGION(startedFastThreadLock);
		isFastThreadStarted = TRUE;
	END_REGION;
}

void StopFastThread(){
	CRITICAL_REGION(startedFastThreadLock);
		isFastThreadStarted = FALSE;
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
	action->Removed();
}

void RemoveElevatorAction(Action* action){
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

void ExecuteActionQueues(F32 dt){

	Action* chassisAction = chassisActionQueue.front();
	if(!(chassisAction->Update(dt))){
		RemoveChassisAction(chassisAction);
	}

	Action* elevatorAction = elevatorActionQueue.front();
	if(!(elevatorAction->Update(dt))){
		RemoveElevatorAction(elevatorAction);
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
	F64 targetMSPerFrame = 1000.0 / targetHz;
	F64 startTime = SystemTime();
	F64 lastTime = SystemTime();

	while(IsFastThreadStarted()){
		if(IsFastThreadRunning()){
			//Processing actions


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
		COUT("Last Fast Thread frame time: %.04fms (%.04fHz).", frameTimeMS, Hz);
	}

	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	COUT("[ELON] Total Fast Thread time: %dm%.04fs.", totalMinutes, totalSeconds);
	return 0;
}

I32 CoreThreadRuntime(U32 targetHz, B32_FUNCPTR runnerCallback, EXE_FUNCPTR executableCallback, ELON* elon){
	F64 targetMSPerFrame = 1000.0 / targetHz;
	F64 lastTime = SystemTime();

	while((elon->*runnerCallback)()){
		//Executing user function
		(elon->*executableCallback)();

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
		}

		F64 endTime = SystemTime();
		F64 frameTimeMS = endTime - lastTime;
		lastTime = endTime;
		F64 Hz = 1000.0/ frameTimeMS;

		//Frame logging
		COUT("Last Core Thread frame time: %.04fms (%.04fHz).", frameTimeMS, Hz);
	}
	return 0;
}

