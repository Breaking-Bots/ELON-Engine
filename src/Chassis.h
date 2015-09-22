#ifndef ELON_CHASSIS_H
#define ELON_CHASSIS_H

#include "Util.h"

/**
 * Initialize chassis and motors
 */
void InitializeChassis(U32 frontLeft, U32 backLeft, U32 frontRight, U32 backRight, U32 gyroPort, U32 gyroSensitivity);

/**
 * Free chassis memory
 */
void TerminateChassis();

/** Mainly used in Autonomous
 * mgntd - forward component of magnitude sent to motors
 * curve - rate of turn, constant for differing forward speeds
 */
void RawDrive(F32 mgntd, F32 curve);

/**
 * leftMgntd - magnitude of left motors
 * rightMgntd - magnitude of right motors
 */
void TankDrive(F32 leftMgntd, F32 rightMgntd);

/**
 * fwdMgntd - magnitude of robot moving forward
 * turnMgntd - magnitude of turning of robot
 */
void ELONDrive(F32 fwdMgntd, F32 turnMgntd);

/**
 * Sets global magnitude of all motors
 */
void SetChassisMagnitude(F32 magnitude);

/**
 * Enables movement of chassis
 */
void EnableChassis(B32 enable);

/**
 * Stops all motors
 */
void StopMotors();

/**
 * Inverts motor that has been inserted backwards
 */
void InvertMotor(U32 motorPort);

/**
 * Returns angle that the robot is facing in degrees
 */
F32 HeadingDeg();

/**
 * Returns angle that the robot is facing in radians
 */
F32 HeadingRad();

/**
 * Writes motor values to motor controllers
 */
void UpdateChassis();


#endif
