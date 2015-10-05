#ifndef ELON_H
#define ELON_H

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
void UpdateInput(ELONMemory* memory);

/*******************************************************************
 * ELON Class				                                       *
 *******************************************************************/

/**
 * ELON class that inherits SampleRobot fro WPILIB, required for WPILIB
 */
class ELON : public SampleRobot{
	//TODO: Make these values
	Task* fastThread;
	B32 autonomousInit = False;
	B32 teleopInit = False;
	B32 testInit = False;
	B32 disabledInit = False;
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
