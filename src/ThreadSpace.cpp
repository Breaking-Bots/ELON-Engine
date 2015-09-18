/*
 * ThreadSpace.cpp
 *
 *  Created on: Sep 18, 2015
 *      Author: Zaeem
 */

#include "WPILib.h"
#include "ThreadSpace.h"

MUTEX_ID startedFastThreadLock;
MUTEX_ID runningFastThreadLock;

B32 isFastThreadStarted;
B32 isFastThreadRunning;


void InitializeThreadSpace(){
	startedFastThreadLock = initializeMutexNormal();
	runningFastThreadLock = initializeMutexNormal();
	isFastThreadStarted = true;
	isFastThreadRunning = true;
}

void TerminateThreadSpace(){
	isFastThreadStarted = false;
	isFastThreadRunning = false;
	deleteMutex(runningFastThreadLock);
	deleteMutex(startedFastThreadLock);
}

B32 IsFastThreadStarted(){
	CRITICAL_REGION(startedFastThreadLock)
		return isFastThreadStarted;
	END_REGION;
}

B32 IsFastThreadRunning(){
	CRITICAL_REGION(runningFastThreadLock)
		return isFastThreadRunning;
	END_REGION;
}

void PauseFastThread(){
	CRITICAL_REGION(runningFastThreadLock)
		isFastThreadRunning = FALSE;
	END_REGION;
}

void ResumeFastThread(){
	CRITICAL_REGION(runningFastThreadLock)
		isFastThreadRunning = TRUE;
	END_REGION;
}

void StartFastThread(){
	CRITICAL_REGION(startedFastThreadLock)
		isFastThreadStarted = TRUE;
	END_REGION;
}

void StopFastThread(){
	CRITICAL_REGION(startedFastThreadLock)
		isFastThreadStarted = FALSE;
	END_REGION;
}

I32 FastThreadRuntime(U32 targetHz){
	F64 targetMSPerFrame = 1000.0 / targetHz;
	F64 startTime = SystemTime();
	F64 lastTime = SystemTime();

	while(IsFastThreadStarted()){
		if(IsFastThreadRunning()){
			//Processing commands

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

I32 CoreThreadRuntime(U32 targetHz, B32_FUNCPTR runnerCallback, EXE_FUNCPTR executableCallback){
	F64 targetMSPerFrame = 1000.0 / targetHz;
	F64 lastTime = SystemTime();

	while((*runnerCallback)){
		//Executing user function
		(*executableCallback);

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

