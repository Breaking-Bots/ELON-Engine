#ifndef ELON_INPUT_H
#define ELON_INPUT_H

#include "Util.h"

/**
 * Controller input type
 */
enum InputType{
	LINEAR, QUADRATIC, QUARTIC
};

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
 * Gets InputType (LINEAR, QUADRATIC, QUARTIC)
 */
InputType GetInputType(U32 gamepadPort);

/**
 * Sets Inputtype for give controller
 * LINEAR provied linear control at all speeds
 * QUADRATIC provides more control at lower speeds (DEFAULT)
 * QUARTIC provides most control at lower speeds
 */
void SetInputType(U32 gamepadPort, InputType inputType);

/**
 * Updates the gamepads from data recieved from driver station
 */
void UpdateInput();

#endif
