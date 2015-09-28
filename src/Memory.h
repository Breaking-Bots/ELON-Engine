#ifndef ELON_MEMORY_H
#define ELON_MEMORY_H

#include "Util.h"
#include "Properties.h"
#include "Input.h"

struct ELONMemory;
struct ELONState;
struct ChassisState;
struct ElevatorState;

struct ELONMemory{
	B32 isInitialized;
	U32 permanentStorageSize;
	void* permanentStorage; //REQUIRED to be cleared to zero at startup
	U32 transientStorageSize;
	void* transientStorage; //REQUIRED to be cleared to zero at startup
};

struct ChassisState{
	F32 motorValues[CHASSIS_NUM_MOTORS]; //Array of motor speed values
	F32 chassisMagnitude; //Magnitude of chassis speed
	F32 sensitivity; //Sensitivity of RawDrive
	U32 nMotors = CHASSIS_NUM_MOTORS; //Number of used motors
	B32 isInitialized; //Initialization flag
	B32 chassisEnabled; //Allow chassis control flag
	I8 invertedMotors[CHASSIS_NUM_MOTORS]; //Array of motor inversions

};

struct ElevatorState{
	F32 motorValue; //Motor speed value
	F32 elevatorMagnitude; //Magnitude of elevator speed
	B32 isInitialized; //Initialization flag
	I8 invertedMotor; //Motor inversion
};

struct ELONState{
	ChassisState chassisState;
	ElevatorState elevatorState;
	Gamepad gamepads[NUM_GAMEPADS];
};




#endif
