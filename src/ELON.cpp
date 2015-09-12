/*
 * ELON.cpp
 *
 *  Created on: Sep 10, 2015
 *      Author: Zaeem
 */

#include "WPILib.h"
#include "Util.h"
#include "Input.h"

class ELON: public SampleRobot
{

F32 targetSecondsPerFrame;

public:

	ELON(){
		//System startup
		targetSecondsPerFrame = 1.0f/(F32)20;

		//Input initialization
		InitializeInput(1);
		SetGamepadPorts(0);

		//Chassis initialization
		InitializeChassis(4);
		SetMotorPorts(0, 1, 2, 3);
		InitializeMotors();

	}

	void Autonomous()
	{

	}

	void OperatorControl()
	{
		Timer* timer = new Timer();
		timer->Start();
		F64 startTime = timer->Get();
		F64 lastTime = timer->Get();

		while(isOperatorControl() && isEnabled()){

			//Input processing
			UpdateInput();

			//Executing commands

			//Time processing
			F64 workTime = timer->Get();
			F64 workSecondsElapsed = workTime - lastTime;

			F64 secondsElapsedForFrame = workSecondsElapsed;
			if(secondsElapsedForFrame < targetSecondsPerFrame){
				Wait(targetSecondsPerFrame - secondsElapsedForFrame);
				F64 testSecondsElapsedForFrame = timer->Get() - lastTime;
				if(testSecondsElapsedForFrame > 0){
					std::cerr << "Waited Too Long." << std::endl;
				}
				while(secondsElapsedForFrame < targetSecondsPerFrame){
					secondsElapsedForFrame = timer->Get() - lastTime;
				}
			}else{
				//TODO: MISSED FRAME
				//TODO: Log
			}

			F64 endTime = timer->Get();
			F64 frameTimeMS = 1000.0 * (endTime - lastTime);
			lastTime = endTime;
			F64 Hz = 1000.0/ frameTimeMS;

			//Frame logging
			std::cout << "Last frame time: " << frameTimeMS << "ms (" << Hz << "Hz)." << std::endl;
		}

		F64 totalTimeElapsed = timer->Get() - startTime;
		//TODO: Log
		std::cout << "Total teleoprator time: " << totalTimeElapsed << "s." << std::endl;


		delete timer;
	}

	void Test()
	{

	}

	~ELON(){
		//System shutdown
		TerminateChassis();
		TerminateInput();
	}
};

U32 main(U32 argc, I8** argv) {
	if (!HALInitialize()){
		std::cerr<<"FATAL ERROR: HAL could not be initialized"<<std::endl;
		return -1;
	}
	HALReport(HALUsageReporting::kResourceType_Language, HALUsageReporting::kLanguage_CPlusPlus);

	ELON *elon = new ELON();
	RobotBase::robotSetup(elon);
	return 0;
}
