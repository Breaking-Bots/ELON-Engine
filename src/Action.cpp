/*
 * Action.cpp
 *
 *  Created on: Sep 14, 2015
 *      Author: Zaeem
 */

#include "WPILib.h"
#include "Action.h"

Action::Action():isInitialized(FALSE){

}

Action::~Action(){

}

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



void *ActionThread(U32 targetHz){
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
				std::cerr << "[ERROR] Action Thread waited too long." << std::endl;
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
		//std::cout << "[ELON] Last action frame time: " << frameTimeMS << "ms (" << Hz << "Hz)." << std::endl;
	}

	F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
	U32 totalMinutes = totalTimeElapsedSeconds / 60;
	F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
	//TODO: Log
	std::cout << "[ELON] Total Action Thread time: " << totalMinutes << "m" << totalSeconds << "s." << std::endl;
}

