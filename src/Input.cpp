/*
 * Input.cpp
 *
 *  Created on: Sep 11, 2015
 *      Author: Zaeem
 */

#include "WPILib.h"
#include "Input.h"

#define _LX 0
#define _LY 1
#define _LT 2
#define _RT 3
#define _RX 4
#define _RY 5
#define NUM_BUTTONS 10

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
		};
	};
	I32 dpad;
	InputType inputType = InputType::QUADRATIC;
};

U32 nGamepads; //Number of active gamepads
DriverStation* ds; //Driverstation where gamepads are connected
Gamepad* gamepads; //Array of gamepads
U32* gamepadPorts; //Array of gamepad ports

F32 deadzone = 0.25f;
F32 deadzone2 = deadzone * deadzone;
F32 triggerDeadzone = 0.06f;


void InitializeInput(U32 numGamepads){
	nGamepads = numGamepads;
	ds = DriverStation::GetInstance();
	gamepads = new Gamepad[nGamepads];
	gamepadPorts = new U32[nGamepads];
}

void SetGamepadPorts(U32 primary){
	if(nGamepads == 1){
		gamepadPorts[0] = primary;
	}else{
		std::cerr << "[ERROR] Amount of gamepad ports do not match number of gamepads: " << nGamepads << "." << std::endl;
	}
}

void SetGamepadPorts(U32 primary, U32 secondary){
	if(nGamepads == 2){
		gamepadPorts[0] = primary;
		gamepadPorts[1] = secondary;
	}else{
		std::cerr << "[ERROR] Amount of gamepad ports do not match number of gamepads: " << nGamepads << "." << std::endl;
	}
}

void TerminateInput(){
	if(gamepadPorts){
		delete[] gamepadPorts;
	}
	if(gamepads){
		delete[] gamepads;
	}
	ds = NULL;
}

U32 Buttons(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].buttons;
		}
	}
	return 0; //TODO: controller doesn't exist
}

B32 A(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].a;
		}
	}
	return 0; //TODO: controller doesn't exist
}

B32 B(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].b;
		}
	}
	return 0; //TODO: controller doesn't exist
}

B32 X(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].x;
		}
	}
	return 0; //TODO: controller doesn't exist
}

B32 Y(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].y;
		}
	}
	return 0; //TODO: controller doesn't exist
}

B32 LB(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].lb;
		}
	}
	return 0; //TODO: controller doesn't exist
}

B32 RB(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].rb;
		}
	}
	return 0; //TODO: controller doesn't exist
}

B32 BACK(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].back;
		}
	}
	return 0; //TODO: controller doesn't exist
}

B32 START(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].start;
		}
	}
	return 0; //TODO: controller doesn't exist
}

B32 L3(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].l3;
		}
	}
	return 0; //TODO: controller doesn't exist
}

B32 R3(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].r3;
		}
	}
	return 0; //TODO: controller doesn't exist
}

F32 LX(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].lx;
		}
	}
	return 0; //TODO: controller doesn't exist
}

F32 LY(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].ly;
		}
	}
	return 0; //TODO: controller doesn't exist
}

F32 RX(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].rx;
		}
	}
	return 0; //TODO: controller doesn't exist
}

F32 RY(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].ry;
		}
	}
	return 0; //TODO: controller doesn't exist
}

F32 LT(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].lt;
		}
	}
	return 0; //TODO: controller doesn't exist
}

F32 RT(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].rt;
		}
	}
	return 0; //TODO: controller doesn't exist
}

I32 DPAD(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].dpad;
		}
	}
	return 0; //TODO: controller doesn't exist
}

InputType GetInputType(U32 gamepadPort){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			return gamepads[i].inputType;
		}
	}
	return InputType::QUADRATIC; //TODO: controller doesn't exist
}


void SetInputType(U32 gamepadPort, InputType inputType){
	for(U32 i = 0; i < nGamepads; i++){
		if(gamepadPorts[i] == gamepadPort){
			gamepads[i].inputType = inputType;
		}
	}
}

void UpdateInput(){
	for(U32 i = 0; i < nGamepads; i++){
		gamepads[i].buttons = ds->GetStickButtons(gamepadPorts[i]);

		//Circular deadzone processing of left joystick
		F32 lx = ds->GetStickAxis(gamepadPorts[i], _LX);
		F32 ly = ds->GetStickAxis(gamepadPorts[i], _LY);
		F32 lmgntd2 = lx*lx+ly*ly;

		if(lmgntd2 < deadzone2){
			gamepads[i].lx = 0.0f;
			gamepads[i].ly = 0.0f;
		}else{
			F32 lmgntd = sqrtf(lmgntd2);
			F32 nlxFactor = lx / lmgntd;
			F32 nlyFactor = ly / lmgntd;
			if(lmgntd > 1.0f){
				lmgntd = 1.0f;
			}
			lmgntd -= deadzone;
			lx = nlxFactor * lmgntd;
			ly = nlyFactor * lmgntd;
			if(gamepads[i].inputType == InputType::LINEAR){
				gamepads[i].lx = lx / (1 - deadzone * nlxFactor);
				gamepads[i].ly = ly / (1 - deadzone * nlyFactor);
			}else if(gamepads[i].inputType == InputType::QUADRATIC){
				gamepads[i].lx = Sq(lx / (1 - deadzone * nlxFactor * Sgn(lx))) * Sgn(lx);
				gamepads[i].ly = Sq(ly / (1 - deadzone * nlyFactor * Sgn(ly))) * Sgn(ly);
			}else if(gamepads[i].inputType == InputType::QUARTIC){
				gamepads[i].lx = Qu(lx / (1 - deadzone * nlxFactor * Sgn(lx))) * Sgn(lx);
				gamepads[i].ly = Qu(ly / (1 - deadzone * nlyFactor * Sgn(ly))) * Sgn(ly);
			}
		}

		//Circular deadzone processing of right joystick
		F32 rx = ds->GetStickAxis(gamepadPorts[i], _RX);
		F32 ry = ds->GetStickAxis(gamepadPorts[i], _RY);
		F32 rmgntd2 = rx*rx+ry*ry;

		if(rmgntd2 < deadzone2){
			gamepads[i].rx = 0.0f;
			gamepads[i].ry = 0.0f;
		}else{
			F32 rmgntd = sqrtf(rmgntd2);
			F32 nrxFactor = rx / rmgntd;
			F32 nryFactor = ry / rmgntd;
			if(rmgntd > 1.0f){
				rmgntd = 1.0f;
			}
			rmgntd -= deadzone;
			rx = nrxFactor * rmgntd;
			ry = nryFactor * rmgntd;
			if(gamepads[i].inputType == InputType::LINEAR){
				gamepads[i].rx = rx / (1 - deadzone * nrxFactor * Sgn(rx));
				gamepads[i].ry = ry / (1 - deadzone * nryFactor * Sgn(ry));
			}else if(gamepads[i].inputType == InputType::QUADRATIC){
				gamepads[i].rx = Sq(rx / (1 - deadzone * nrxFactor * Sgn(rx))) * Sgn(rx);
				gamepads[i].ry = Sq(ry / (1 - deadzone * nryFactor * Sgn(ry))) * Sgn(ry);
			}else if(gamepads[i].inputType == InputType::QUARTIC){
				gamepads[i].rx = Qu(rx / (1 - deadzone * nrxFactor * Sgn(rx))) * Sgn(rx);
				gamepads[i].ry = Qu(ry / (1 - deadzone * nryFactor * Sgn(ry))) * Sgn(ry);
			}
		}

		//Linear deadzone processing of left trigger
		F32 lt = ds->GetStickAxis(gamepadPorts[i], _LT);
		if(lt < triggerDeadzone){
			gamepads[i].lt = 0.0f;
		}else{
			if(gamepads[i].inputType == InputType::LINEAR){
				gamepads[i].lt = (lt - triggerDeadzone * Sgn(lt))/(1.0f - triggerDeadzone);
			}else if(gamepads[i].inputType == InputType::QUADRATIC){
				gamepads[i].lt = Sq((lt - triggerDeadzone * Sgn(lt))/(1.0f - triggerDeadzone));
			}else if(gamepads[i].inputType == InputType::QUARTIC){
				gamepads[i].lt = Qu((lt - triggerDeadzone * Sgn(lt))/(1.0f - triggerDeadzone));
			}
		}

		//Linear deadzone processing of right trigger
		F32 rt = ds->GetStickAxis(gamepadPorts[i], _RT);
		if(rt < triggerDeadzone){
			gamepads[i].rt = 0.0f;
		}else{
			if(gamepads[i].inputType == InputType::LINEAR){
				gamepads[i].rt = (rt - triggerDeadzone * Sgn(rt))/(1.0f - triggerDeadzone);
			}else if(gamepads[i].inputType == InputType::QUADRATIC){
				gamepads[i].rt = Sq((rt - triggerDeadzone * Sgn(rt))/(1.0f - triggerDeadzone));
			}else if(gamepads[i].inputType == InputType::QUARTIC){
				gamepads[i].rt = Qu((rt - triggerDeadzone * Sgn(rt))/(1.0f - triggerDeadzone));
			}
		}


		//Stash POV (D-PAD) angles in degrees
		gamepads[i].dpad = ds->GetStickPOV(gamepadPorts[i], 0);
	}
}
