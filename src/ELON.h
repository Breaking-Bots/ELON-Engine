#ifndef ELON_H
#define ELON_H

#include "Util.h"
#include "Memory.h"

class Task;

class ELON : public SampleRobot{
	Task* fastThread;
public:
	ELONMemory elonMemory;

	ELON();

	void RobotInit();

	void Autonomous();

	void OperatorControl();

	void Test();

	void Disabled();

	~ELON();
};

#endif
