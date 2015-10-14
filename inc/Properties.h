#ifndef ELON_PROPERTIES_H
#define ELON_PROPERTIES_H

#include "Util.h"

#define RECORD_STATE 0

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
#define ELONInputRecordingDataFile ("/home/lvuser/InputRecording.eid")
#define ELONTeleopRecordingDataFileName ("/home/lvuser/TeleopRecording")

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

#endif
