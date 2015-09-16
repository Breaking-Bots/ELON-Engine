#ifndef ELON_ACTION_H
#define ELON_ACTION_H

#include "Util.h"

class Action{

	B32 isRunning;
	B32 isInitialized;
	F64 startTime;

public:

	Action();

	B32 Update(F32 dt);

	void StartTimer();

	virtual void Initialize() = 0;

	virtual void Execute(F32 dt) = 0;

	virtual B32 TerminationCondition();

	virtual void Terminate() = 0;

	virtual ~Action();

};


void *ActionThread(U32 targetHz);

#endif
