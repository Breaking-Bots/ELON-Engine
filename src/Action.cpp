/*
 * Action.cpp
 *
 *  Created on: Sep 14, 2015
 *      Author: Zaeem
 */

#include "WPILib.h"
#include "Action.h"
#include "Util.h"
#include "ThreadSpace.h"

Action::Action(){

}

Action::~Action(){

}

B32 Action::IsCanceled() const{
	return isCanceled;
}

B32 Action::IsRunning() const{
	return isRunning;
}

void Action::StartActionFromFastThread() {
	isRunning = TRUE;
	startTime = -1;
}

void Action::StartTimer(){
	startTime = SystemTime();
}

void Action::Removed(){
	if(isInitialized){
		if(isCanceled){
			Interupted();
		}else{
			Terminate();
		}
	}
	isInitialized = FALSE;
	isCanceled = FALSE;
	isRunning = FALSE;
}

B32 Action::Update(F32 dt){
	if(!isInitialized){
		Initialize();
		StartTimer();
	}

	Execute(dt);
	return !TerminationCondition();
}

void StartChassisAction(Action* action){
	BufferChassisAction(action);
}

void StartElevatorAction(Action* action){
	BufferElevatorAction(action);
}

