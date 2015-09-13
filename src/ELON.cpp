/*
 * ELON.cpp
 *
 *  Created on: Sep 10, 2015
 *      Author: Zaeem
 */

#include "WPILib.h"
#include "Util.h"
#include "Input.h"
#include "Properties.h"

inline F64 SystemTime(){
	return GetFPGATime() * 1000.0;
}

class ELON: public SampleRobot
{

F32 targetSecondsPerFrame;

public:

	ELON(){
		//System startup
		targetSecondsPerFrame = 1.0f/LOOP_HZ;

		//Input initialization
		InitializeInput(1);
		SetGamepadPorts(0);

		//Chassis initialization
		InitializeChassis(0, 1, 2, 3);

	}

	void Autonomous()
	{

	}

	void OperatorControl()
	{
		F64 startTime = SystemTime();
		F64 lastTime = SystemTime();

		while(IsOperatorControl() && IsEnabled()){

			//Input processing
			UpdateInput();
			F32 lx = LX(0);
			F32 ly = LY(0);
			F32 rx = RX(0);
			F32 ry = RY(0);
			F32 lt = LT(0);
			F32 rt = RT(0);
			std::cout << lx << "|" << ly << "|"<< rx << "|"<< ry << "|"<< lt << "|"<< rt << "." << std::endl;

			//Updating subsystems
			UpdateChassis();

			//Time processing
			F64 workTime = SystemTime();
			F64 workSecondsElapsed = workTime - lastTime;

			F64 secondsElapsedForFrame = workSecondsElapsed;
			if(secondsElapsedForFrame < targetSecondsPerFrame){
				Wait(targetSecondsPerFrame - secondsElapsedForFrame);
				F64 testSecondsElapsedForFrame = SystemTime() - lastTime;
				if(testSecondsElapsedForFrame > 0){
					std::cerr << "Waited Too Long." << std::endl;
				}
				while(secondsElapsedForFrame < targetSecondsPerFrame){
					secondsElapsedForFrame = SystemTime() - lastTime;
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
			std::cout << "Last frame time: " << frameTimeMS << "ms (" << Hz << "Hz)." << std::endl;
		}

		F64 totalTimeElapsed = (SystemTime() - startTime) * 1000.0;
		//TODO: Log
		std::cout << "Total teleoprator time: " << totalTimeElapsed << "s." << std::endl;
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
