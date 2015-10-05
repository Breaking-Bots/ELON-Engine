/*
 * Input.cpp
 *
 *  Created on: Sep 26, 2015
 *      Author: Zaeem
 */

#include "../inc/ELONEngine.h"

U32 Buttons(Gamepad* gamepad){
	return gamepad->buttons;
}

B32 A(Gamepad* gamepad){
	return gamepad->a;
}

B32 B(Gamepad* gamepad){
	return gamepad->b;
}

B32 X(Gamepad* gamepad){
	return gamepad->x;
}

B32 Y(Gamepad* gamepad){
	return gamepad->y;
}

B32 LB(Gamepad* gamepad){
	return gamepad->lb;
}

B32 RB(Gamepad* gamepad){
	return gamepad->rb;
}

B32 BACK(Gamepad* gamepad){
	return gamepad->back;
}

B32 START(Gamepad* gamepad){
	return gamepad->start;
}

B32 L3(Gamepad* gamepad){
	return gamepad->l3;
}

B32 R3(Gamepad* gamepad){
	return gamepad->r3;
}

B32 ATapped(Gamepad* gamepad){
	return gamepad->a && !gamepad->_a;
}

B32 BTapped(Gamepad* gamepad){
	return gamepad->b && !gamepad->_b;
}

B32 XTapped(Gamepad* gamepad){
	return gamepad->x && !gamepad->_x;
}

B32 YTapped(Gamepad* gamepad){
	return gamepad->y && !gamepad->_y;
}

B32 LBTapped(Gamepad* gamepad){
	return gamepad->lb && !gamepad->_lb;
}

B32 RBTapped(Gamepad* gamepad){
	return gamepad->rb && !gamepad->_rb;
}

B32 BACKTapped(Gamepad* gamepad){
	return gamepad->back && !gamepad->_back;
}

B32 STARTTapped(Gamepad* gamepad){
	return gamepad->start && !gamepad->_start;
}

B32 L3Tapped(Gamepad* gamepad){
	return gamepad->l3 && !gamepad->_l3;
}

B32 R3Tapped(Gamepad* gamepad){
	return gamepad->r3 && !gamepad->_r3;
}

B32 AReleased(Gamepad* gamepad){
	return !gamepad->a  && gamepad->_a;
}

B32 BReleased(Gamepad* gamepad){
	return !gamepad->b  && gamepad->_b;
}

B32 XReleased(Gamepad* gamepad){
	return !gamepad->x  && gamepad->_x;
}

B32 YReleased(Gamepad* gamepad){
	return !gamepad->y  && gamepad->_y;
}

B32 LBReleased(Gamepad* gamepad){
	return !gamepad->lb  && gamepad->_lb;
}

B32 RBReleased(Gamepad* gamepad){
	return !gamepad->rb  && gamepad->_rb;
}

B32 BACKReleased(Gamepad* gamepad){
	return !gamepad->back  && gamepad->_back;
}

B32 STARTReleased(Gamepad* gamepad){
	return !gamepad->start  && gamepad->_start;
}

B32 L3Released(Gamepad* gamepad){
	return !gamepad->l3  && gamepad->_l3;
}

B32 R3Released(Gamepad* gamepad){
	return !gamepad->r3  && gamepad->_r3;
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

I32 DPAD(Gamepad* gamepad){
	return gamepad->dpad;
}

InputType GetInputType(Gamepad* gamepad){
	return gamepad->inputType;
}


void SetInputType(Gamepad* gamepad, InputType inputType){
	gamepad->inputType = inputType;
}
