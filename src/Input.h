#ifndef ELON_INPUT_H
#define ELON_INPUT_H

/**
 * gamepad input type
 */
enum InputType{
	LINEAR, QUADRATIC, QUARTIC
};

/**
 * Gamepad struct holds all necessary analog and digital inputs
 */
struct Gamepad{
	union{
		F32 analog[6];
		struct{
			F32 lx;
			F32 ly;
			F32 lt;
			F32 rt;
			F32 rx;
			F32 ry;
		};
	};
	union{
		U32 buttons;
		struct{
			U32 a : 1;
			U32 b : 1;
			U32 x : 1;
			U32 y : 1;
			U32 lb : 1;
			U32 rb : 1;
			U32 back : 1;
			U32 start : 1;
			U32 l3 : 1;
			U32 r3 : 1;
			U32 padding : 6;
			U32 _a : 1;
			U32 _b : 1;
			U32 _x : 1;
			U32 _y : 1;
			U32 _lb : 1;
			U32 _rb : 1;
			U32 _back : 1;
			U32 _start : 1;
			U32 _l3 : 1;
			U32 _r3 : 1;
		};
	};
	I32 dpad;
	InputType inputType = InputType::QUADRATIC;
};

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

F32 LX(Gamepad* gamepad);

F32 LY(Gamepad* gamepad);

F32 RX(Gamepad* gamepad);

F32 RY(Gamepad* gamepad);

F32 LT(Gamepad* gamepad);

F32 RT(Gamepad* gamepad);

I32 DPAD(Gamepad* gamepad);

InputType GetInputType(Gamepad* gamepad);

#endif
