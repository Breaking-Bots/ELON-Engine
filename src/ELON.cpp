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
#include "Chassis.h"
#include "Elevator.h"

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
		InitializeChassis(CHASSIS_PORTS);

		//Elevator initialization
		InitializeElevator(ELEVATOR_PORT);

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
			std::cout << "[ELON] " << lx << "|" << ly << "|"<< rx << "|"<< ry << "|"<< lt << "|"<< rt << "." << std::endl;

			if(START(0)){
				EnableChassis(TRUE);
			}else if(BACK(0)){
				EnableChassis(FALSE);
			}

			ELONDrive(-LY(0), RX(0));
			Elevate(RB(0) - LB(0));

			F32 chassisMagnitude = SystemMagnitudeInterpolation(MIN_SPEED, DEF_SPEED, MAX_SPEED, RT(0) - LT(0));
			F32 elevatorMagnitude = Coserp(DEF_SPEED, MAX_SPEED, NormalizeAlpha(RT(0) - LT(0)));

			SetChassisMagnitude(chassisMagnitude);
			SetElevatorMagnitude(elevatorMagnitude);

			//Updating subsystems
			UpdateChassis();
			UpdateElevator();

			//Time processing
			F64 workTime = SystemTime();
			F64 workSecondsElapsed = workTime - lastTime;

			F64 secondsElapsedForFrame = workSecondsElapsed;
			if(secondsElapsedForFrame < targetSecondsPerFrame){
				Wait(targetSecondsPerFrame - secondsElapsedForFrame);
				F64 testSecondsElapsedForFrame = SystemTime() - lastTime;
				if(testSecondsElapsedForFrame > 0){
					std::cerr << "[ERROR] Waited Too Long." << std::endl;
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
			std::cout << "[ELON] Last frame time: " << frameTimeMS << "ms (" << Hz << "Hz)." << std::endl;
		}

		F64 totalTimeElapsed = (SystemTime() - startTime) * 1000.0;
		//TODO: Log
		std::cout << "[ELON] Total teleoprator time: " << totalTimeElapsed << "s." << std::endl;
	}

	void Test()
	{

	}

	~ELON(){
		//System shutdown
		TerminateElevator();
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
