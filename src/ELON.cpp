#include "WPILib.h"
#include "typedefs.h"

class ELON: public SampleRobot
{

F32 targetSecondsPerFrame;

public:

	ELON(){
		//System startup
		targetSecondsPerFrame = 1.0f/(F32)20;

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

			//Executing commands

			//Time processing
			F64 workTime = timer->Get();
			F64 workSecondsElapsed = workTime - lastTime;

			F64 secondsElapsedForFrame = workSecondsElapsed;
			if(secondsElapsedForFrame < targetSecondsPerFrame){
				Wait(targetSecondsPerFrame - secondsElapsedForFrame);
				F64 testSecondsElapsedForFrame = timer->Get() - lastTime;
				if(testSecondsElapsedForFrame > 0) std::cout << "Waited Too Long" << std::endl;
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
		}

		F64 totalTimeElapsed = timer->Get() - startTime;
		//TODO: Log


		delete timer;
	}

	void Test()
	{

	}

	~ELON(){
		//System shutdown
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
