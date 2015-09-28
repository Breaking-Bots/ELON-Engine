#ifndef ELON_CHASSIS_H
#define ELON_CHASSIS_H

#include "Util.h"

struct ChassisState;

/** Mainly used in Autonomous
 * mgntd - forward component of magnitude sent to motors
 * curve - rate of turn, constant for differing forward speeds
 */
void RawDrive(ChassisState* state, F32 mgntd, F32 curve);

/**
 * leftMgntd - magnitude of left motors
 * rightMgntd - magnitude of right motors
 */
void TankDrive(ChassisState* state, F32 leftMgntd, F32 rightMgntd);

/**
 * fwdMgntd - magnitude of robot moving forward
 * turnMgntd - magnitude of turning of robot
 */
void ELONDrive(ChassisState* state, F32 fwdMgntd, F32 turnMgntd);

/**
 * Sets global magnitude of all motors
 */
void SetChassisMagnitude(ChassisState* state, F32 magnitude);

/**
 * Enables movement of chassis
 */
void EnableChassis(ChassisState* state, B32 enable);

/**
 * Returns whether or not the chassis is enabled
 */
B32 IsChassisEnabled(ChassisState* state);

/**
 * Stops all motors
 */
void StopMotors(ChassisState* state);

/**
 * Inverts motor that has been inserted backwards
 */
void InvertMotor(ChassisState* state, U32 motorPort);

#endif
