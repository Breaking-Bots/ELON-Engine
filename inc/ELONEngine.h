#ifndef ELON_ENGINE_H
#define ELON_ENGINE_H

/*******************************************************************
 * Properties				                                       *
 *******************************************************************/

#define RECORD_STATE 0
#define NUM_REPLAY_BUFFERS 4
#define NUM_AUTON_BUFFERS 1

#define DISABLE_FAST_THREAD 1
#define DISABLE_CORE_THREAD 0

#define CORE_THREAD_HZ 50
#define FAST_THREAD_HZ 200

#define NUM_GAMEPADS 1

#define CHASSIS_NUM_MOTORS 4
#define CHASSIS_PORT_FL 0
#define CHASSIS_PORT_BL 1
#define CHASSIS_PORT_FR 2
#define CHASSIS_PORT_BR 3
#define CHASSIS_PORTS CHASSIS_PORT_FL, CHASSIS_PORT_BL, CHASSIS_PORT_FR, CHASSIS_PORT_BR

#define ELEVATOR_PORT 4

#define GYRO_PORT 0

#define MIN_SPEED (0.35f)
#define DEF_SPEED (0.65f)
#define MAX_SPEED (1.00f)

#define GYRO_SENSITIVITY (0.006f)

#define ELONEngineBinary ("/home/lvuser/libELON.so")
#define ELONEngineTempBinary ("/home/lvuser/libELON_temp.so")
#define ELONInputRecordingDataFileName ("/home/lvuser/InputRecording")
#define ELONTeleopRecordingDataFileName ("/home/lvuser/TeleopRecording")

#define ELONAutonomousDataFileName_0 ("/home/lvuser/Autonomous.eid")

#define _A 0
#define _B 1
#define _X 2
#define _Y 3
#define _LB 4
#define _RB 5
#define _BACK 6
#define _START 7
#define _L3 8
#define _R3 9
#define NUM_BUTTONS 10

#define _LX 0
#define _LY 1
#define _LT 2
#define _RT 3
#define _RX 4
#define _RY 5
#define NUM_AXES 6

#define _UP 0
#define _DOWN 1
#define _LEFT 2
#define _RIGHT 3
#define NUM_DPAD 4

#define DEADZONE (0.25f)
#define DEADZONE_SQ (DEADZONE * DEADZONE)
#define TRIGGER_DEADZONE (0.06f)

/*******************************************************************
 * Util						                                       *
 *******************************************************************/

#include <cstdint>
#include <cfloat>
#include <cmath>
#include <string>

#ifdef __cplusplus
extern "C"
{
#endif

#define lscope static
#define glob static
#define intern static
#define scast static_cast
#define rcast reinterpret_cast

typedef int32_t B32;

typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef float F32;
typedef double F64;

#define I8_MIN INT8_MIN
#define I16_MIN INT16_MIN
#define I32_MIN INT32_MIN
#define I64_MIN INT64_MIN
#define F32_MIN FLT_MIN
#define F64_MIN DBL_MIN

#define I8_MAX INT8_MAX
#define I16_MAX INT16_MAX
#define I32_MAX INT32_MAX
#define I64_MAX INT64_MAX
#define U8_MAX UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX
#define F32_MAX FLT_MAX
#define F64_MAX DBL_MAX
#define PI (3.1415926535897932f)
#define TAU (6.2831853071795865f)
#define E (2.71828182845904523526f)
#define RAD_TO_DEG (57.2957795f)
#define DEG_TO_RAD (0.0174532925f)
#define True (0xFFFFFFFF)
#define False (0x00000000)
#define EPSILON (0.00001f)
#define EZERO (0.000000001f)

#define sizeofArr(arr) (sizeof((arr)) / sizeof((arr)[0]))

#define AlignPo2(value, alignment) ((value + ((alignment) - 1)) & ~((alignment) - 1))
#define Align4(value) ((value + 3) & ~3)
#define Align8(value) ((value + 7) & ~7)
#define Align16(value) ((value + 15) & ~15)

#define F32_CALLBACK_F32_F32_F32_F32(name) F32 name(F32 a, F32 b, F32 c, F32 d)
typedef F32_CALLBACK_F32_F32_F32_F32(F32CallbackF32F32F32F32);

#define F32_CALLBACK_F32_F32_F32(name) F32 name(F32 a, F32 b, F32 c)
typedef F32_CALLBACK_F32_F32_F32(F32CallbackF32F32F32);

#define F32_CALLBACK_F32_F32(name) F32 name(F32 a, F32 b)
typedef F32_CALLBACK_F32_F32(F32CallbackF32F32);

#define F32_CALLBACK_F32(name) F32 name(F32 x)
typedef F32_CALLBACK_F32(F32CallbackF32);

#define I32_CALLBACK_F32(name) I32 name(F32 x)
typedef I32_CALLBACK_F32(I32CallbackF32);

#define U64_CALLBACK_U32(name) U64 name(U32 x)
typedef U64_CALLBACK_U32(U64CallbackU32);

/**
 * Logging dytor
 */
#define LOGGING_CALLBACK(name) I32 name(const std::string& format, ...)
typedef LOGGING_CALLBACK(LoggingCallback);

/**
 * System Time dytor
 */
#define SYSTEM_TIME_CALLBACK(name) F64 name()
typedef SYSTEM_TIME_CALLBACK(SystemTimeCallback);

#ifdef __cplusplus
}
#endif

/*******************************************************************
 * Input					                                       *
 *******************************************************************/

/**
 * Gamepad input type
 */
enum InputType{
	LINEAR, QUADRATIC, QUARTIC
};

/**
 * Gamepad button state
 */
struct ButtonState{
	U32 halfTransitionCount;
	B32 endedDown;
};

/**
 * Gamepad struct holds all necessary analog and digital inputs
 */
struct Gamepad{
	union{
		F32 analog[NUM_AXES];
		struct{
			F32 lx;
			F32 ly;
			F32 lt;
			F32 rt;
			F32 rx;
			F32 ry;

			//All analogs must be added above this line
			F32 analogTerminator;
		};
	};
	union{
		ButtonState buttons[NUM_BUTTONS];
		struct{
			ButtonState a;
			ButtonState b;
			ButtonState x;
			ButtonState y;
			ButtonState lb;
			ButtonState rb;
			ButtonState back;
			ButtonState start;
			ButtonState l3;
			ButtonState r3;

			//All buttons must be added above this line
			ButtonState buttonTerminator;
		};
	};
	union{
		ButtonState dpad[NUM_DPAD];
		struct{
			ButtonState up;
			ButtonState down;
			ButtonState left;
			ButtonState right;

			//All dpad buttons must be added above this line
			ButtonState dpadTerminator;
		};
	};

	InputType inputType = InputType::QUADRATIC;
};


struct Input{
	Gamepad gamepads[NUM_GAMEPADS];
};

inline Gamepad* GetGamepad(Input* input, U32 port){
	if(port > sizeofArr(input->gamepads)){
		return &(input->gamepads[0]);
	}
	return &(input->gamepads[port]);
}

U32 Buttons(Gamepad* gamepad);

B32 Button(Gamepad* gamepad, U32 buttonIndex);

B32 ButtonTapped(Gamepad* gamepad, U32 buttonIndex);

B32 ButtonHeld(Gamepad* gamepad, U32 buttonIndex);

B32 ButtonReleased(Gamepad* gamepad, U32 buttonIndex);

B32 ButtonPostRealeased(Gamepad* gamepad, U32 buttonIndex);

B32 DPAD(Gamepad* gamepad, U32 dpadIndex);

B32 DPADTapped(Gamepad* gamepad, U32 dpadIndex);

B32 DPADHeld(Gamepad* gamepad, U32 dpadIndex);

B32 DPADReleased(Gamepad* gamepad, U32 dpadIndex);

B32 DPADPostRealeased(Gamepad* gamepad, U32 dpadIndex);

F32 Analog(Gamepad* gamepad, U32 stickIndex);

InputType GetInputType(Gamepad* gamepad);

void SetInputType(Gamepad* gamepad, InputType inputType);

/*******************************************************************
 * Memory					                                       *
 *******************************************************************/

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
	I32 invertedMotors[CHASSIS_NUM_MOTORS]; //Array of motor inversions
	F32 lastGyroAngleDeg;
	F32 gyroAngleDeg;
};

struct ElevatorState{
	F32 motorValue; //Motor speed value
	F32 elevatorMagnitude; //Magnitude of elevator speed
	B32 isInitialized; //Initialization flag
	I32 invertedMotor; //Motor inversion
};

struct ELONState{
	ChassisState chassisState;
	ElevatorState elevatorState;
};

/*******************************************************************
 * Chassis					                                       *
 *******************************************************************/

struct ELONMemory;

/** Mainly used in Autonomous
 * mgntd - forward component of magnitude sent to motors
 * curve - rate of turn, constant for differing forward speeds
 */
void RawDrive(ELONMemory* memory, F32 mgntd, F32 curve);

/**
 * leftMgntd - magnitude of left motors
 * rightMgntd - magnitude of right motors
 */
void TankDrive(ELONMemory* memory, F32 leftMgntd, F32 rightMgntd);

/**
 * fwdMgntd - magnitude of robot moving forward
 * turnMgntd - magnitude of turning of robot
 */
void ELONDrive(ELONMemory* memory, F32 fwdMgntd, F32 turnMgntd);

/**
 * Sets global magnitude of all motors
 */
void SetChassisMagnitude(ELONMemory* memory, F32 magnitude);

/**
 * Enables movement of chassis
 */
void EnableChassis(ELONMemory* memory, B32 enable);

/**
 * Returns whether or not the chassis is enabled
 */
B32 IsChassisEnabled(ELONMemory* memory);

/**
 * Stops all motors
 */
void StopMotors(ELONMemory* memory);

/**
 * Inverts motor that has been inserted backwards
 */
void InvertMotor(ELONMemory* memory, U32 motorPort);

/*******************************************************************
 * Elevator					                                       *
 *******************************************************************/

/**
 * Set motor speed from -1.0f to 1.0f
 */
void Elevate(ELONMemory* memory, F32 speed);

/**
 * Set global magnitude of elevator motor
 */
void SetElevatorMagnitude(ELONMemory* memory, F32 magnitude);

/**
 * Invert the motor
 */
void InvertElevator(ELONMemory* memory);

/*******************************************************************
 * Actions					                                       *
 *******************************************************************/



/*******************************************************************
 * ELON Engine 				                                       *
 *******************************************************************/

//TODO: Make this two types, one for Init, one for iterative callback

/**
 * Called at 50Hz when in teleop mode
 */
#define ELON_CALLBACK(name) void name(ELONMemory* memory, Input* input)
typedef ELON_CALLBACK(ELONCallback);

#endif
