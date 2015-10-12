/*
 * Input.cpp
 *
 *  Created on: Sep 26, 2015
 *      Author: Zaeem
 */

#include "ELONEngine.h"

U32 Buttons(Gamepad* gamepad){
	U32 buttons = 0;
	for(U32 i = 0; i < NUM_BUTTONS; i++){
		buttons |= buttons & (gamepad->buttons[i].endedDown << i);
	}
	return buttons;
}

B32 Button(Gamepad* gamepad, U32 buttonIndex){
	return (gamepad->buttons[buttonIndex].endedDown);
}

B32 ButtonTapped(Gamepad* gamepad, U32 buttonIndex){
	return (gamepad->buttons[buttonIndex].endedDown && (gamepad->buttons[buttonIndex].halfTransitionCount));
}

B32 ButtonHeld(Gamepad* gamepad, U32 buttonIndex){
	return (gamepad->buttons[buttonIndex].endedDown && !gamepad->buttons[buttonIndex].halfTransitionCount);
}

B32 ButtonReleased(Gamepad* gamepad, U32 buttonIndex){
	return (!gamepad->buttons[buttonIndex].endedDown && (gamepad->buttons[buttonIndex].halfTransitionCount));
}

B32 ButtonPostRealeased(Gamepad* gamepad, U32 buttonIndex){
	return (!gamepad->buttons[buttonIndex].endedDown && !gamepad->buttons[buttonIndex].halfTransitionCount);
}

B32 DPAD(Gamepad* gamepad, U32 dpadIndex){
	return (gamepad->dpad[dpadIndex].endedDown);
}

B32 DPADTapped(Gamepad* gamepad, U32 dpadIndex){
	return (gamepad->dpad[dpadIndex].endedDown && (gamepad->dpad[dpadIndex].halfTransitionCount));
}

B32 DPADHeld(Gamepad* gamepad, U32 dpadIndex){
	return (gamepad->dpad[dpadIndex].endedDown && !gamepad->dpad[dpadIndex].halfTransitionCount);
}

B32 DPADReleased(Gamepad* gamepad, U32 dpadIndex){
	return (!gamepad->dpad[dpadIndex].endedDown && (gamepad->dpad[dpadIndex].halfTransitionCount));
}

B32 DPADPostRealeased(Gamepad* gamepad, U32 dpadIndex){
	return (!gamepad->dpad[dpadIndex].endedDown && !gamepad->dpad[dpadIndex].halfTransitionCount);
}

F32 Analog(Gamepad* gamepad, U32 stickIndex) {
	return gamepad->analog[stickIndex];
}

InputType GetInputType(Gamepad* gamepad){
	return gamepad->inputType;
}

void SetInputType(Gamepad* gamepad, InputType inputType){
	gamepad->inputType = inputType;
}
