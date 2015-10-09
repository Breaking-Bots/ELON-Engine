#ifndef ELON_INPUT_H
#define ELON_INPUT_H

/**
 * Gamepad input type
 */
enum InputType{
	LINEAR, QUADRATIC, QUARTIC
};

/**
 * Gamepad button state
 */
struct ButtonState{
	I32 halfTransitionCount;
	B32 endedDown;
};

/**
 * Gamepad struct holds all necessary analog and digital inputs
 */
struct Gamepad{
	union{
		F32 analog[NUM_AXES];
		struct{
			F32 lx;
			F32 ly;
			F32 lt;
			F32 rt;
			F32 rx;
			F32 ry;

			//All axes must be added above this line
			F32 axisTerminator;
		};
	};
	union{
		ButtonState buttons[NUM_BUTTONS];
		struct{
			ButtonState a;
			ButtonState b;
			ButtonState x;
			ButtonState y;
			ButtonState lb;
			ButtonState rb;
			ButtonState back;
			ButtonState start;
			ButtonState l3;
			ButtonState r3;

			//All buttons must be added above this line
			ButtonState buttonTerminator;
		};
	};
	union{
		ButtonState dpad[NUM_DPAD];
		struct{
			ButtonState up;
			ButtonState down;
			ButtonState left;
			ButtonState right;

			//All dpad buttons must be added above this line
			ButtonState dpadTerminator;
		};
	};

	InputType inputType = InputType::QUADRATIC;
};


struct Input{
	Gamepad gamepads[NUM_GAMEPADS];
};

inline Gamepad* GetGamepad(Input* input, U32 port){
	if(port > sizeofArr(input->gamepads)){
		return &(input->gamepads[0]);
	}
	return &(input->gamepads[port]);
}

U32 Buttons(Gamepad* gamepad);

B32 A(Gamepad* gamepad);

B32 B(Gamepad* gamepad);

B32 X(Gamepad* gamepad);

B32 Y(Gamepad* gamepad);

B32 LB(Gamepad* gamepad);

B32 RB(Gamepad* gamepad);

B32 BACK(Gamepad* gamepad);

B32 START(Gamepad* gamepad);

B32 L3(Gamepad* gamepad);

B32 R3(Gamepad* gamepad);

B32 ATapped(Gamepad* gamepad);

B32 BTapped(Gamepad* gamepad);

B32 XTapped(Gamepad* gamepad);

B32 YTapped(Gamepad* gamepad);

B32 LBTapped(Gamepad* gamepad);

B32 RBTapped(Gamepad* gamepad);

B32 BACKTapped(Gamepad* gamepad);

B32 STARTTapped(Gamepad* gamepad);

B32 L3Tapped(Gamepad* gamepad);

B32 R3Tapped(Gamepad* gamepad);

B32 AReleased(Gamepad* gamepad);

B32 BReleased(Gamepad* gamepad);

B32 XReleased(Gamepad* gamepad);

B32 YReleased(Gamepad* gamepad);

B32 LBReleased(Gamepad* gamepad);

B32 RBReleased(Gamepad* gamepad);

B32 BACKReleased(Gamepad* gamepad);

B32 STARTReleased(Gamepad* gamepad);

B32 L3Released(Gamepad* gamepad);

B32 R3Released(Gamepad* gamepad);

B32 UP(Gamepad* gamepad);

B32 DOWN(Gamepad* gamepad);

B32 LEFT(Gamepad* gamepad);

B32 RIGHT(Gamepad* gamepad);

F32 LX(Gamepad* gamepad);

F32 LY(Gamepad* gamepad);

F32 RX(Gamepad* gamepad);

F32 RY(Gamepad* gamepad);

F32 LT(Gamepad* gamepad);

F32 RT(Gamepad* gamepad);


InputType GetInputType(Gamepad* gamepad);

#endif
