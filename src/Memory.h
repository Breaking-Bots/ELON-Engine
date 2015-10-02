#ifndef ELON_MEMORY_H
#define ELON_MEMORY_H

#include "Util.h"
#include "Properties.h"
#include "Input.h"

#ifdef __cplusplus
extern "C" {
#endif

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
	LoggingCallback* Cout;
	LoggingCallback* Cerr;
	SystemTimeCallback* SystemTime;
	F32CallbackF32F32F32* Clamp;
	F32CallbackF32F32* Max;
	F32CallbackF32* Sq;
	F32CallbackF32* Cu;
	F32CallbackF32* Qu;
	I32CallbackF32* Sgn;
	F32CallbackF32* NormalizeAlpha;
	F32CallbackF32F32F32* Lerp;
	F32CallbackF32F32F32* Coserp;
	F32CallbackF32F32F32F32* SystemMagnitudeInterpolation;
	F32CallbackF32* PrincipalAngleDeg;
	F32CallbackF32* PrincipalAngleRad;
	F32CallbackF32* MinDistAngleDeg;
	F32CallbackF32* MinDistAngleRad;
	F32CallbackF32F32* AngularDistDeg;
	F32CallbackF32F32* AngularDistRad;
	U64CallbackU32* Pow10;
	U64CallbackU32* DecToBin;
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

#ifdef __cplusplus
}
#endif


#endif
