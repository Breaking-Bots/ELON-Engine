#ifndef ELON_ACTIONS_H
#define ELON_ACTIONS_H

#include "Util.h"

struct ELONMemory;

/**
 * Abstract class that represents an action that a subsystem can undergo.
 * Must be inherited to create custom actions.
 */
class Action{

	ELONMemory* memory;

	F64 startTime = -1;
	B32 isRunning = False;
	B32 isInitialized = False;
	B32 isCanceled = False;
	F64 timeout = F64_MAX;
public:

	/**
	 * Action constructor, takes in timeout
	 */
	Action(ELONMemory* memory, F64 timeout = F64_MAX);

	/**
	 * ONLY TO BE CALLED FROM FAST THREAD
	 * Starts action from the fast thread
	 */
	void StartActionFromFastThread();

	/**
	 * ONLY TO BE CALLED FROM FAST THREAD
	 * Runs one iteration of an action with a given delta time
	 */
	B32 Update(F32 dt);

	/**
	 * States that the action is removed from the fast thread queue
	 * Runs all termination sequences
	 */
	void Removed();

	/**
	 * Cancels the object, runs the interupted termination sequence
	 */
	void Cancel();

	/**
	 * Returns whether or not the action is canceled
	 */
	B32 IsCanceled() const;

	/**
	 * Returns whether or not the action is running
	 */
	B32 IsRunning() const;

	/**
	 * Returns whether or not the action has taken the allocated amount of time
	 */
	B32 IsTimedOut() const;

	/**
	 * Starts timer, used for timed actions
	 */
	void StartTimer();

	/**
	 * Pure virtual action initialization for custom child actions
	 */
	virtual void Initialize() = 0;

	/**
	 * Pure virtual action execution sequence for custom child actions
	 * Runs once per iteration of fast thread with a given delta time
	 */
	virtual void Execute(F32 dt) = 0;

	/**
	 * Pure virtual action termination coditions for custom child actions
	 * User specified flags that tell the fast thread that this action should end
	 */
	virtual B32 TerminationCondition() = 0;

	/**
	 * Pure virtual action interuption sequence for custom child actions
	 * Gets called when action is canceled
	 */
	virtual void Interupted() = 0;

	/**
	 * Pure virtual action termination for custom child actions
	 */
	virtual void Terminate() = 0;

	/**
	 * Virtual action destructor
	 */
	virtual ~Action();

};

/**
 * Adds action parameter to the chassis action buffer
 */
void StartChassisAction(Action* action);

/**
 * Adds action parameter to the elevator action buffer
 */
void StartElevatorAction(Action* action);

#endif
