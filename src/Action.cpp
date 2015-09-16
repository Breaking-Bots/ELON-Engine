/*
 * Action.cpp
 *
 *  Created on: Sep 14, 2015
 *      Author: Zaeem
 */

#include "WPILib.h"
#include "Action.h"
#include "stdarg.h"
#include "HAL/cpp/Synchronized.hpp"


void Action::StartTimer(){
	startTime = SystemTime();
}

B32 Action::Update(F32 dt){
	if(!isInitialized){
		Initialize();
		StartTimer();
	}

	Execute(dt);
	return !TerminationCondition();
}

I32 ActionThread(...){
	U32 targetHz = 200;
	F64 targetMSPerFrame = 1000.0 / targetHz;
	F64 startTime = SystemTime();
	F64 lastTime = SystemTime();

	while(TRUE){
		//Processing commands

		//Updating subsystems

		//Time processing
		F64 workMSElapsed = SystemTime() - lastTime;
		if(workMSElapsed < targetMSPerFrame){
			Wait((targetMSPerFrame - workMSElapsed * 1000.0));
			F64 testMSElapsedForFrame = SystemTime() - lastTime;
			if(testMSElapsedForFrame > targetMSPerFrame){
				CERR("Action Thread waited too long.");
			}
			do{
				workMSElapsed = SystemTime() - lastTime;
			} while(workMSElapsed < targetMSPerFrame);
		}else{
			//TODO: MISSED FRAME
			//TODO: Log
		}

		F64 endTime = SystemTime();
		F64 frameTimeMS = endTime - lastTime;
		lastTime = endTime;
		F64 Hz = 1000.0/ frameTimeMS;

		//Frame logging
		COUT("Last Action Thread frame time: %.04fms (%.04fHz).", frameTimeMS, Hz);
	}

	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	COUT("[ELON] Total Action Thread time: %dm%.04fs.", totalMinutes, totalSeconds);
	return 0;
}

