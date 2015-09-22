#ifndef ELON_INPUT_H
#define ELON_INPUT_H

#include "Util.h"

/**
 * gamepad input type
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

/**
 * Returns whether or not the A button is pressed given the gamepad port
 */
B32 A(U32 gamepadPort);

/**
 * Returns whether or not the B button is pressed given the gamepad port
 */
B32 B(U32 gamepadPort);

/**
 * Returns whether or not the X button is pressed given the gamepad port
 */
B32 X(U32 gamepadPort);

/**
 * Returns whether or not the Y button is pressed given the gamepad port
 */
B32 Y(U32 gamepadPort);

/**
 * Returns whether or not the LB button is pressed given the gamepad port
 */
B32 LB(U32 gamepadPort);

/**
 * Returns whether or not the RB button is pressed given the gamepad port
 */
B32 RB(U32 gamepadPort);

/**
 * Returns whether or not the BACK button is pressed given the gamepad port
 */
B32 BACK(U32 gamepadPort);

/**
 * Returns whether or not the START button is pressed given the gamepad port
 */
B32 START(U32 gamepadPort);

/**
 * Returns whether or not the L3 button is pressed given the gamepad port
 */
B32 L3(U32 gamepadPort);

/**
 * Returns whether or not the R3 button is pressed given the gamepad port
 */
B32 R3(U32 gamepadPort);

/**
 * Returns whether or not the A button is tapped given the gamepad port
 */
B32 ATapped(U32 gamepadPort);

/**
 * Returns whether or not the B button is tapped given the gamepad port
 */
B32 BTapped(U32 gamepadPort);

/**
 * Returns whether or not the X button is tapped given the gamepad port
 */
B32 XTapped(U32 gamepadPort);

/**
 * Returns whether or not the Y button is tapped given the gamepad port
 */
B32 YTapped(U32 gamepadPort);

/**
 * Returns whether or not the LB button is tapped given the gamepad port
 */
B32 LBTapped(U32 gamepadPort);

/**
 * Returns whether or not the RB button is tapped given the gamepad port
 */
B32 RBTapped(U32 gamepadPort);

/**
 * Returns whether or not the BACK button is tapped given the gamepad port
 */
B32 BACKTapped(U32 gamepadPort);

/**
 * Returns whether or not the START button is tapped given the gamepad port
 */
B32 STARTTapped(U32 gamepadPort);

/**
 * Returns whether or not the L3 button is tapped given the gamepad port
 */
B32 L3Tapped(U32 gamepadPort);

/**
 * Returns whether or not the R3 button is tapped given the gamepad port
 */
B32 R3Tapped(U32 gamepadPort);

/**
 * Returns whether or not the A button is released given the gamepad port
 */
B32 AReleased(U32 gamepadPort);

/**
 * Returns whether or not the B button is released given the gamepad port
 */
B32 BReleased(U32 gamepadPort);

/**
 * Returns whether or not the X button is released given the gamepad port
 */
B32 XReleased(U32 gamepadPort);

/**
 * Returns whether or not the Y button is released given the gamepad port
 */
B32 YReleased(U32 gamepadPort);

/**
 * Returns whether or not the LB button is released given the gamepad port
 */
B32 LBReleased(U32 gamepadPort);

/**
 * Returns whether or not the RB button is released given the gamepad port
 */
B32 RBReleased(U32 gamepadPort);

/**
 * Returns whether or not the BACK button is released given the gamepad port
 */
B32 BACKReleased(U32 gamepadPort);

/**
 * Returns whether or not the START button is released given the gamepad port
 */
B32 STARTReleased(U32 gamepadPort);

/**
 * Returns whether or not the L3 button is released given the gamepad port
 */
B32 L3Released(U32 gamepadPort);

/**
 * Returns whether or not the RB button is released given the gamepad port
 */
B32 R3Released(U32 gamepadPort);

/**
 * Returns the normalized value [-1,1] of the left stick's X value given the gamepad port
 */
F32 LX(U32 gamepadPort);

/**
 * Returns the normalized value [-1,1] of the left stick's Y value given the gamepad port
 */
F32 LY(U32 gamepadPort);

/**
 * Returns the normalized value [-1,1] of the right stick's X value given the gamepad port
 */
F32 RX(U32 gamepadPort);

/**
 * Returns the normalized value [-1,1] of the right stick's Y value given the gamepad port
 */
F32 RY(U32 gamepadPort);

/**
 * Returns the normalized value [-1,1] of the left trigger given the gamepad port
 */
F32 LT(U32 gamepadPort);

/**
 * Returns the normalized value [-1,1] of the right trigger given the gamepad port
 */
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
 * Sets InputType for givengamepadr
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
