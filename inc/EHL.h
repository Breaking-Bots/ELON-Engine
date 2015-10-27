#ifndef ELON_H
#define ELON_H

#ifdef __cplusplus
extern "C" {
#endif

class Task;

typedef void* MODULE;

typedef I32 HANDLE;

/*******************************************************************
 * Util					                                           *
 *******************************************************************/

/**
 * Get FGPA Time in milliseconds
 */
SYSTEM_TIME_CALLBACK(SystemTime);

/**
 * Clamps value of a between b and c
 */
F32_CALLBACK_F32_F32_F32(Clamp);

/**
 * Returns the greater value of a and b
 */
F32_CALLBACK_F32_F32(Max);

/**
 * Returns the value of x squared
 */
F32_CALLBACK_F32(Sq);

/**
 * Returns the value of x cubed
 */
F32_CALLBACK_F32(Cu);

/**
 * Returns the value of x quarted
 */
F32_CALLBACK_F32(Qu);

/**
 * Return sign of x
 */
I32_CALLBACK_F32(Sgn);

/**
 * Normalizes x of interval [-1,1] to interval [0,1]
 */
F32_CALLBACK_F32(NormalizeAlpha);

/**
 * Linearly interpolates between a and b in the scale of value c of interval [0,1]
 */
F32_CALLBACK_F32_F32_F32(Lerp);

/**
 * Calculates cosine interpolation between a and b in the scale of value c of interval [0,1]
 */
F32_CALLBACK_F32_F32_F32(Coserp);

/**
 * Calculates the system magnitude by interpolation between a, b, c in the scale of d of interval [-1,1]
 */
F32_CALLBACK_F32_F32_F32_F32(SystemMagnitudeInterpolation);

/**
 * Calculates principle angle in degrees from interval [-INFINITY,INFINITY] to interval [0,360]
 */
F32_CALLBACK_F32(PrincipalAngleDeg);

/**
 * Calculates principle angle in radians from interval [-INFINITY,INFINITY] to interval [0,TAU]
 */
F32_CALLBACK_F32(PrincipalAngleRad);

/**
 * Calculate minimum distance angle in degrees from interval [-INFINITY,INFINITY] to interval [-180,180]
 */
F32_CALLBACK_F32(MinDistAngleDeg);

/**
 * Calculate minimum distance angle in radians from interval [-INFINITY,INFINITY] to interval [-PI,PI]
 */
F32_CALLBACK_F32(MinDistAngleRad);

/**
 * Calculates the shortest angular distance in degrees between two angles of interval [-INFINITY,INFINITY] to [-180,180]
 */
F32_CALLBACK_F32_F32(AngularDistDeg);

/**
 * Calculates the shortest angular distance in radians between two angles of interval [-INFINITY,INFINITY] to [-PI,PI]
 */
F32_CALLBACK_F32_F32(AngularDistRad);

/**
 * Return 10 raised to the exponent of 10
 */
U64_CALLBACK_U32(Pow10);

/**
 * Converts decimal integer to binary (buggy)
 */
U64_CALLBACK_U32(DecToBin);

/**
 * Represents file
 */
struct File{
	void* data;
	U32 size;
};

U32 GetFileSize(std::string filename, I32 fd = 0, B32 useFD = False, B32 ignoreFailure = False);

/**
 * Reads entire file and returns File struct
 */
File ReadEntireFile(std::string filename, B32 ignoreFailure = False);

/**
 * Writes memory to file of given filename, creates file if doesn't exist
 */
B32 WriteEntireFile(std::string filename, U32 memorySize, void* memory, B32 ignoreFailure = False);

/**
 * Copies one file to another
 * TODO: optimize or find better solution
 */
B32 CopyFile(std::string src, std::string dest, B32 ignoreFailure = False);

/**
 * Returns last time a file was written to
 */
I64 GetLastWriteTime(std::string filename, B32 ignoreFailure = False);

/*******************************************************************
 * ELON Engine Management                                          *
 *******************************************************************/

struct ELONEngine{
	MODULE module;
	I64 lastWriteTime;
	ELONCallback* InitTeleop;
	ELONCallback* TeleopCallback;
	ELONCallback* InitTest;
	ELONCallback* TestCallback;
	ELONCallback* InitAutonomous;
	ELONCallback* AutonomousCallback;
	ELONCallback* InitDisabled;
	ELONCallback* DisabledCallback;
	B32 isValid;
};

ELON_CALLBACK(ELONCallbackStub);

/**
 * Loads ELONEngine
 */
ELONEngine LoadELONEngine(std::string filename);

/**
 * Unloads ELONEngine
 */
void UnloadELONEngine(ELONEngine* engine);

/*******************************************************************
 * Elevator		                                                   *
 *******************************************************************/

/**
 * Initialize elevator motor
 */
void InitializeElevator();

/**
 * Writes motor value to motor
 */
void UpdateElevator(ELONMemory* memory);

/**
 * Free elevator memory
 */
void TerminateElevator();

/*******************************************************************
 * Chassis		                                                   *
 *******************************************************************/

/**
 * Initialize chassis and motors
 */
void InitializeChassis();

/**
 * Writes motor values to motor controllers
 */
void UpdateChassis(ELONMemory* memory);

/**
 * Free chassis memory
 */
void TerminateChassis();

/*******************************************************************
 * Thread Space                                                    *
 *******************************************************************/

#if 0

/**
 * Initialize the thread space
 */
void InitializeThreadSpace();

/**
 * Terminates the thread space
 */
void TerminateThreadSpace();

/**
 * Checks if fast thread has started
 * Thread safe
 */
B32 IsFastThreadStarted();

/**
 * Checks if fast thread is running
 * Thread safe
 */
B32 IsFastThreadRunning();

/**
 * Pauses fast thread
 * Thread safe
 */
void PauseFastThread();

/**
 * Resumes fast thread
 * Thread safe
 */
void ResumeFastThread();

/**
 * Starts fast thread
 * Thread safe
 */
void StartFastThread();

/**
 * Stops fast thread
 * Thread safe
 */
void StopFastThread();

/**
 * Add an object to the chassis action buffer
 * Thread safe
 */
void BufferChassisAction(Action* action);

/**
 * Add an object to the elevator action buffer
 * Thread safe
 */
void BufferElevatorAction(Action* action);

/**
 * Removes action from chassis' queue of actions
 */
void RemoveChassisAction(Action* action);

/**
 * Removes action from elevator's queue of actions
 */
void RemoveElevatorAction(Action* action);

/**
 * Executes actions in queues and adds the buffered actions to global queues
 * Thread safe
 */
void ExecuteActionQueues(F32 dt);

/**
 * Function callback for running the fast thread runtime on a separate thread
 */
I32 FastThreadRuntime(U32 targetHz);

#endif

/*******************************************************************
 * Input					                                       *
 *******************************************************************/

/**
 * Updates the gamepads from data recieved from driver station
 */
void UpdateInput(DriverStation* ds, ELONMemory* memory);

#ifdef __cplusplus
}
#endif

/*******************************************************************
 * ELON Hardware Layer		                                       *
 *******************************************************************/

struct EHLReplayBuffer{
	HANDLE fileHandle;
	char filename[256];
	void* memoryBlock;
	U32 offset = 0;
};

struct EHLState{
	U32 totalSize;
	void* totalELONMemoryBlock;
	EHLReplayBuffer replayBuffers[NUM_REPLAY_BUFFERS];
	EHLReplayBuffer autonBuffers[NUM_AUTON_BUFFERS];

	HANDLE autonRecordingHandle;
	U32 autonRecordingIndex;
	B32 recordingAuton = False;
	U32 autonCyclesCounter = 0;

	HANDLE autonPlayBackHandle;
	B32 playingAuton = False;
	U32 autonPlayBackIndex;

	HANDLE recordingHandle;
	U32 inputRecordingIndex;

	HANDLE playBackHandle;
	U32 inputPlayBackIndex;

	HANDLE lastTeleopRecordingHandle;
	U32 lastTeleopRecordingIndex;
};

struct EHLRecordedInput{
	U32 inputCount;
	Input* inputStream;
};


/*******************************************************************
 * ELON Class				                                       *
 *******************************************************************/

/**
 * ELON class that inherits SampleRobot from WPILIB, required for WPILIB
 */
class ELON : public SampleRobot{
	//TODO: Make these values
	Task* fastThread;
public:
	ELON();

	void RobotInit();

	void RobotMain();

	void Autonomous();

	void OperatorControl();

	void Test();

	void Disabled();

	~ELON();
};


#endif
