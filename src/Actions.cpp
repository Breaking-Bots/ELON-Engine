/*
 * Action.cpp
 *
 *  Created on: Sep 14, 2015
 *      Author: Zaeem
 */

#include "ELONEngine.h"

Action::Action(ELONMemory* memory, F64 timeout):memory(memory), timeout(timeout){

}

Action::~Action(){

}

B32 Action::IsTimedOut() const{
	return memory->SystemTime() - startTime >= timeout;
}

B32 Action::IsCanceled() const{
	return isCanceled;
}

B32 Action::IsRunning() const{
	return isRunning;
}

void Action::StartActionFromFastThread() {
	isRunning = True;
	startTime = -1;
}

void Action::StartTimer(){
	startTime = memory->SystemTime();
}

void Action::Removed(){
	if(isInitialized){
		if(isCanceled){
			Interupted();
		}else{
			Terminate();
		}
	}
	isInitialized = False;
	isCanceled = False;
	isRunning = False;
}

B32 Action::Update(F32 dt){
	if(!isInitialized){
		Initialize();
		StartTimer();
	}

	Execute(dt);
	return !TerminationCondition() || IsTimedOut();
}

void StartChassisAction(Action* action){
	//BufferChassisAction(action);
}

void StartElevatorAction(Action* action){
	//BufferElevatorAction(action);
}

