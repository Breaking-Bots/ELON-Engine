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
	U32 halfTransitionCount;
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

			//All analogs must be added above this line
			F32 analogTerminator;
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

B32 Button(Gamepad* gamepad, U32 buttonIndex);

B32 ButtonTapped(Gamepad* gamepad, U32 buttonIndex);

B32 ButtonHeld(Gamepad* gamepad, U32 buttonIndex);

B32 ButtonReleased(Gamepad* gamepad, U32 buttonIndex);

B32 ButtonPostRealeased(Gamepad* gamepad, U32 buttonIndex);

B32 DPAD(Gamepad* gamepad, U32 dpadIndex);

B32 DPADTapped(Gamepad* gamepad, U32 dpadIndex);

B32 DPADHeld(Gamepad* gamepad, U32 dpadIndex);

B32 DPADReleased(Gamepad* gamepad, U32 dpadIndex);

B32 DPADPostRealeased(Gamepad* gamepad, U32 dpadIndex);

F32 Analog(Gamepad* gamepad, U32 stickIndex);

InputType GetInputType(Gamepad* gamepad);

void SetInputType(Gamepad* gamepad, InputType inputType);

#endif
