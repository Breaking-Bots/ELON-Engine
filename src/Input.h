#ifndef ELON_INPUT_H
#define ELON_INPUT_H

#include "Util.h"

/**
 * Initialization Order: InitializeInput, SetGamepadPorts
 */
void InitializeInput(U32 numGamepads);
void SetGamepadPorts(U32 primary);
void SetGamepadPorts(U32 primary, U32 secondary);

/**
 * Free input memory
 */
void TerminateInput();

/**
 * Returns a 32-bit integer where the individual bits represent the buttons' states
 */
U32 Buttons(U32 gamepadPort);

B32 A(U32 gamepadPort);
B32 B(U32 gamepadPort);
B32 X(U32 gamepadPort);
B32 Y(U32 gamepadPort);
B32 LB(U32 gamepadPort);
B32 RB(U32 gamepadPort);
B32 BACK(U32 gamepadPort);
B32 START(U32 gamepadPort);
B32 LB(U32 gamepadPort);
B32 RB(U32 gamepadPort);

F32 LX(U32 gamepadPort);
F32 LY(U32 gamepadPort);
F32 RX(U32 gamepadPort);
F32 RY(U32 gamepadPort);
F32 LT(U32 gamepadPort);
F32 RT(U32 gamepadPort);

/**
 * Returns angle of D-PAD in degrees, or -1 if not pressed
 */
I32 DPAD(U32 gamepadPort);

/**
 * Updates the gamepads from data recieved from driver station
 */
void UpdateInput();

#endif
