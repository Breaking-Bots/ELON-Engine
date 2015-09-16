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
#include "Action.h"

class ELON: public SampleRobot
{

F64 targetMSPerFrame;

//TODO: Use pthread.h instead
Task* actionThread;

public:

	ELON(){
		//Thread startup
		actionThread = new Task("ActionThread", ActionThread, 100, KiB(5));

		//System startup
		targetMSPerFrame = 1000.0/LOOP_HZ;

		//Input initialization
		InitializeInput(1);
		SetGamepadPorts(0);

		//Chassis initialization
		InitializeChassis(CHASSIS_PORTS, GYRO_PORT, GYRO_SENSITIVITY);

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
			//std::cout << "[ELON] " << lx << "|" << ly << "|"<< rx << "|"<< ry << "|"<< lt << "|"<< rt << "." << std::endl;

			if(START(0)){
				EnableChassis(TRUE);
			}else if(BACK(0)){
				EnableChassis(FALSE);
			}

			ELONDrive(ly, rx);
			Elevate(RB(0) - LB(0));

			F32 chassisMagnitude = SystemMagnitudeInterpolation(MIN_SPEED, DEF_SPEED, MAX_SPEED, rt - lt);
			F32 elevatorMagnitude = Coserp(DEF_SPEED, MAX_SPEED, NormalizeAlpha(rt - lt));

			SetChassisMagnitude(chassisMagnitude);
			SetElevatorMagnitude(chassisMagnitude);

			//Updating subsystems
			UpdateChassis();
			UpdateElevator();

			//Time processing
			F64 workMSElapsed = SystemTime() - lastTime;
			if(workMSElapsed < targetMSPerFrame){
				Wait((targetMSPerFrame - workMSElapsed * 1000.0));
				F64 testMSElapsedForFrame = SystemTime() - lastTime;
				if(testMSElapsedForFrame > targetMSPerFrame){
					std::cerr << "[ERROR] Core waited too long." << std::endl;
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
			std::cout << "[ELON] Last Core frame time: " << frameTimeMS << "ms (" << Hz << "Hz)." << std::endl;
		}

		F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
		U32 totalMinutes = totalTimeElapsedSeconds / 60;
		F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
		//TODO: Log
		std::cout << "[ELON] Total Teleoperator time: " << totalMinutes << "m" << totalSeconds << "s." << std::endl;
	}

	void Test()
	{
		F64 startTime = SystemTime();
		F64 lastTime = SystemTime();

		while(IsTest() && IsEnabled()){

			//Input processing
			UpdateInput();
			F32 lx = LX(0);
			F32 ly = LY(0);
			F32 rx = RX(0);
			F32 ry = RY(0);
			F32 lt = LT(0);
			F32 rt = RT(0);
			//std::cout << "[ELON] " << lx << "|" << ly << "|"<< rx << "|"<< ry << "|"<< lt << "|"<< rt << "." << std::endl;

			if(START(0)){
				EnableChassis(TRUE);
			}else if(BACK(0)){
				EnableChassis(FALSE);
			}

			ELONDrive(ly, rx);
			Elevate(RB(0) - LB(0));

			F32 chassisMagnitude = SystemMagnitudeInterpolation(MIN_SPEED, DEF_SPEED, MAX_SPEED, rt - lt);
			F32 elevatorMagnitude = Coserp(DEF_SPEED, MAX_SPEED, NormalizeAlpha(rt - lt));

			SetChassisMagnitude(chassisMagnitude);
			SetElevatorMagnitude(chassisMagnitude);

			//Updating subsystems
			UpdateChassis();
			UpdateElevator();

			//Time processing
			F64 workMSElapsed = SystemTime() - lastTime;
			if(workMSElapsed < targetMSPerFrame){
				Wait((targetMSPerFrame - workMSElapsed * 1000.0));
				F64 testMSElapsedForFrame = SystemTime() - lastTime;
				if(testMSElapsedForFrame > targetMSPerFrame){
					std::cerr << "[ERROR] Core waited Too Long." << std::endl;
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
			std::cout << "[ELON] Last Core frame time: " << frameTimeMS << "ms (" << Hz << "Hz)." << std::endl;
		}

		F64 totalTimeElapsedSeconds = (SystemTime() - startTime) * 1000.0;
		U32 totalMinutes = totalTimeElapsedSeconds / 60;
		F32 totalSeconds = totalTimeElapsedSeconds - (totalMinutes * 60.0f);
		//TODO: Log
		std::cout << "[ELON] Total Test time: " << totalMinutes << "m" << totalSeconds << "s." << std::endl;
	}

	void Disabled(){

	}

	~ELON(){
		//System shutdown
		TerminateElevator();
		TerminateChassis();
		TerminateInput();
	}
};

I32 main() {
	if (!HALInitialize()){
		std::cerr<<"[ERROR] HAL could not be initialized."<<std::endl;
		return -1;
	}
	HALReport(HALUsageReporting::kResourceType_Language, HALUsageReporting::kLanguage_CPlusPlus);

	ELON *elon = new ELON();
	RobotBase::robotSetup(elon);
	return 0;
}
