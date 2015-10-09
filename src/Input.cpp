/*
 * Input.cpp
 *
 *  Created on: Sep 26, 2015
 *      Author: Zaeem
 */

#include "../inc/ELONEngine.h"

U32 Buttons(Gamepad* gamepad){
	U32 buttons = 0;
	for(U32 i = 0; i < NUM_BUTTONS; i++){
		buttons |= buttons & (gamepad->buttons[i].endedDown << i);
	}
	return buttons;
}

B32 A(Gamepad* gamepad){
	return gamepad->a.endedDown;
}

B32 B(Gamepad* gamepad){
	return gamepad->b.endedDown;
}

B32 X(Gamepad* gamepad){
	return gamepad->x.endedDown;
}

B32 Y(Gamepad* gamepad){
	return gamepad->y.endedDown;
}

B32 LB(Gamepad* gamepad){
	return gamepad->lb.endedDown;
}

B32 RB(Gamepad* gamepad){
	return gamepad->rb.endedDown;
}

B32 BACK(Gamepad* gamepad){
	return gamepad->back.endedDown;
}

B32 START(Gamepad* gamepad){
	return gamepad->start.endedDown;
}

B32 L3(Gamepad* gamepad){
	return gamepad->l3.endedDown;
}

B32 R3(Gamepad* gamepad){
	return gamepad->r3.endedDown;
}

B32 ATapped(Gamepad* gamepad){
	return gamepad->a.endedDown;
}

B32 BTapped(Gamepad* gamepad){
	return gamepad->b.endedDown;
}

B32 XTapped(Gamepad* gamepad){
	return gamepad->x.endedDown;
}

B32 YTapped(Gamepad* gamepad){
	return gamepad->y.endedDown;
}

B32 LBTapped(Gamepad* gamepad){
	return gamepad->lb.endedDown;
}

B32 RBTapped(Gamepad* gamepad){
	return gamepad->rb.endedDown;
}

B32 BACKTapped(Gamepad* gamepad){
	return gamepad->back.endedDown;
}

B32 STARTTapped(Gamepad* gamepad){
	return gamepad->start.endedDown;
}

B32 L3Tapped(Gamepad* gamepad){
	return gamepad->l3.endedDown;
}

B32 R3Tapped(Gamepad* gamepad){
	return gamepad->r3.endedDown;
}

B32 AReleased(Gamepad* gamepad){
	return !gamepad->a.endedDown;
}

B32 BReleased(Gamepad* gamepad){
	return !gamepad->b.endedDown;
}

B32 XReleased(Gamepad* gamepad){
	return !gamepad->x.endedDown;
}

B32 YReleased(Gamepad* gamepad){
	return !gamepad->y.endedDown;
}

B32 LBReleased(Gamepad* gamepad){
	return !gamepad->lb.endedDown;
}

B32 RBReleased(Gamepad* gamepad){
	return !gamepad->rb.endedDown;
}

B32 BACKReleased(Gamepad* gamepad){
	return !gamepad->back.endedDown;
}

B32 STARTReleased(Gamepad* gamepad){
	return !gamepad->start.endedDown;
}

B32 L3Released(Gamepad* gamepad){
	return !gamepad->l3.endedDown;
}

B32 R3Released(Gamepad* gamepad){
	return !gamepad->r3.endedDown;
}

B32 UP(Gamepad* gamepad){
	return gamepad->up.endedDown;
}

B32 DOWN(Gamepad* gamepad){
	return gamepad->down.endedDown;
}

B32 LEFT(Gamepad* gamepad){
	return gamepad->left.endedDown;
}

B32 RIGHT(Gamepad* gamepad){
	return gamepad->right.endedDown;
}

F32 LX(Gamepad* gamepad){
	return gamepad->lx;
}

F32 LY(Gamepad* gamepad){
	return gamepad->ly;
}

F32 RX(Gamepad* gamepad){
	return gamepad->rx;
}

F32 RY(Gamepad* gamepad){
	return gamepad->ry;
}

F32 LT(Gamepad* gamepad){
	return gamepad->lt;
}

F32 RT(Gamepad* gamepad){
	return gamepad->rt;
}

InputType GetInputType(Gamepad* gamepad){
	return gamepad->inputType;
}


void SetInputType(Gamepad* gamepad, InputType inputType){
	gamepad->inputType = inputType;
}
