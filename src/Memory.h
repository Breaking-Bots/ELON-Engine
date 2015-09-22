#ifndef ELON_MEMORY_H
#define ELON_MEMORY_H

#include "Util.h"

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
	Talon motors[4];
	B32 isInitialized;
	F32 sensitivity;
	U32 nMotors = 4;
	F32 motorValues[4];
	U32 motorPorts[4];
	I8 invertedMotors[4];
	Gyro gyro;
	F32 chassisMagnitude;
	B32 chassisEnabled;

};

struct ElevatorState{

};

struct ELONState{
	ChassisState chassisState;
	ElevatorState elevatorState;
	F32 chassisMagnitude;
	F32 elevatorMagnitude;
};




#endif
