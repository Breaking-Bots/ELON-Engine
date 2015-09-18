#pragma once

class ELON: public SampleRobot
{

Task* fastThread;

public:
	B32 TeleopRunnerCallback();

	B32 TestRunnerCallback();

	B32 AutonomousRunnerCallback();

	void TeleopExecutableCallback();

	void TestExecutableCallback();

	void AutonomousExecutableCallback();


	ELON();

	void Autonomous();

	void OperatorControl();

	void Test();

	void Disabled();

	~ELON();
};
