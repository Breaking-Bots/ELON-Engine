#ifndef ELON_THREAD_SPACE_H
#define ELON_THREAD_SPACE_H

#include "Util.h"

//Forward declaration of action class
class Action;

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

/**
 * Runtime for core thread, runs slower than the fast thread
 * runnerCallback is the function provided that returns a B32 that determines whether the thread should run
 * executableCallback is the function provided that has the code that the user requires to be run at the
 * frequency provided
 * elon is the robot object running
 */
I32 CoreThreadRuntime(U32 targetHz, B32_FUNCPTR runnerCallback, EXE_FUNCPTR executableCallback, ELON* elon);

#endif
