#ifndef ELON_H
#define ELON_H

#include "Util.h"

struct ELONMemory;

class ELON: public SampleRobot
{

Task* fastThread;

public:
ELONMemory* elonMemory;

	ELON();

	void RobotInit();

	void Autonomous();

	void OperatorControl();

	void Test();

	void Disabled();

	~ELON();
};

#endif
