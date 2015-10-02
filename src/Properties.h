#ifndef ELON_PROPERTIES_H
#define ELON_PROPERTIES_H

#include "Util.h"

#define DISABLE_FAST_THREAD 1
#define DISABLE_CORE_THREAD 0

#define CORE_THREAD_HZ 50
#define FAST_THREAD_HZ 200

#define NUM_GAMEPADS 2

#define CHASSIS_NUM_MOTORS 4
#define CHASSIS_PORT_FL 0
#define CHASSIS_PORT_BL 1
#define CHASSIS_PORT_FR 2
#define CHASSIS_PORT_BR 3
#define CHASSIS_PORTS CHASSIS_PORT_FL, CHASSIS_PORT_BL, CHASSIS_PORT_FR, CHASSIS_PORT_BR

#define ELEVATOR_PORT 4

#define GYRO_PORT 0

#define MIN_SPEED (0.30f)
#define DEF_SPEED (0.65f)
#define MAX_SPEED (1.00f)

#define GYRO_SENSITIVITY (0.006f)

#define deadzone (0.25f)
#define deadzone2 (deadzone * deadzone)
#define triggerDeadzone (0.06f)

#endif
