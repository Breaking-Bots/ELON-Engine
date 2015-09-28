#ifndef ELON_H
#define ELON_H

#include "Util.h"
#include "ELONEngine.h"

class Task;

/*******************************************************************
 * ELON Engine Management                                          *
 *******************************************************************/

struct ELONEngine;

/*******************************************************************
 * Elevator		                                                   *
 *******************************************************************/

/**
 * Initialize elevator motor
 */
intern void InitializeElevator();

/**
 * Writes motor value to motor
 */
intern void UpdateElevator(ElevatorState* state);

/**
 * Free elevator memory
 */
intern void TerminateElevator();

/*******************************************************************
 * Chassis		                                                   *
 *******************************************************************/

/**
 * Initialize chassis and motors
 */
intern void InitializeChassis();

/**
 * Writes motor values to motor controllers
 */
intern void UpdateChassis(ChassisState* state);

/**
 * Free chassis memory
 */
intern void TerminateChassis();

/*******************************************************************
 * Thread Space                                                    *
 *******************************************************************/

/**
 * Initialize the thread space
 */
intern void InitializeThreadSpace();

/**
 * Terminates the thread space
 */
intern void TerminateThreadSpace();

/**
 * Checks if fast thread has started
 * Thread safe
 */
intern B32 IsFastThreadStarted();

/**
 * Checks if fast thread is running
 * Thread safe
 */
intern B32 IsFastThreadRunning();

/**
 * Pauses fast thread
 * Thread safe
 */
intern void PauseFastThread();

/**
 * Resumes fast thread
 * Thread safe
 */
intern void ResumeFastThread();

/**
 * Starts fast thread
 * Thread safe
 */
intern void StartFastThread();

/**
 * Stops fast thread
 * Thread safe
 */
intern void StopFastThread();

/**
 * Add an object to the chassis action buffer
 * Thread safe
 */
intern void BufferChassisAction(Action* action);

/**
 * Add an object to the elevator action buffer
 * Thread safe
 */
intern void BufferElevatorAction(Action* action);

/**
 * Removes action from chassis' queue of actions
 */
intern void RemoveChassisAction(Action* action);

/**
 * Removes action from elevator's queue of actions
 */
intern void RemoveElevatorAction(Action* action);

/**
 * Executes actions in queues and adds the buffered actions to global queues
 * Thread safe
 */
intern void ExecuteActionQueues(F32 dt);

/**
 * Function callback for running the fast thread runtime on a separate thread
 */
intern I32 FastThreadRuntime(U32 targetHz);

/**
 * Runtime for core thread, runs slower than the fast thread
 * runnerCallback is the function provided that returns a B32 that determines whether the thread should run
 * executableCallback is the function provided that has the code that the user requires to be run at the
 * frequency provided
 * elon is the robot object running
 */
intern I32 CoreThreadRuntime(U32 targetHz, B32_FUNCPTR runnerCallback, EXE_FUNCPTR executableCallback, ELON* elon);

/*******************************************************************
 * Input					                                       *
 *******************************************************************/

/**
 * Updates the gamepads from data recieved from driver station
 */
intern void UpdateInput(ELONMemory* memory);

/*******************************************************************
 * ELON Class				                                       *
 *******************************************************************/

class ELON : public SampleRobot{
	void* totalElonMemoryBlock;
	//TODO: Make these values
	Task* fastThread;
	ELONEngine* engine;
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

typedef B32 (ELON::*B32_FUNCPTR)();

#endif
