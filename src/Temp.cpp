#ifndef TEMP_CPP
#define TEMP_CPP

extern "C"{

F32 PIDCalculation(ELONMemory* memory, F32 value, F32 target, F32 dt, PIDState* pidState){
	/*F32 steadyError = target - value;
	F32 p = steadyError * kP;

	F32 i = 0;
	F32 d = 0;


	*prev_error = steadyError;

	return p + i + d;*/

	F32& e = pidState->e;
	F32& e1 = pidState->e1;
	F32& e2 = pidState->e2;
	F32 kP = pidState->kP;
	F32 kI = pidState->kI;
	F32 kD = pidState->kD;
	F32& u = pidState->u;
	F32& du = pidState->du;

	F32 a = (kP +kI*(dt/2)+kD/(dt));
	F32 b = (-kP+kI*(dt/2)-2*(kD/dt));
	F32 c = kD/dt;

	e2 = e1;
	e1 = e;
	e = target - value;
	//memory->Cout("%f", value);

	du = a * e + b*e1 + c*e2;
	u += du;

	return u;

}

ELON_CALLBACK(InitTemp){
	if(!memory->isInitialized){
		ELONState* state = scast<ELONState*>(memory->permanentStorage);
		state->chassisState.chassisMagnitude = DEF_SPEED;
		ResetPIDState(&state->chassisState.leftPID);
		ResetPIDState(&state->chassisState.rightPID);
		state->chassisState.leftPID.kP = 0.4f;
		state->chassisState.leftPID.kI = 0.0f;
		state->chassisState.leftPID.kD = 0;
		state->chassisState.rightPID.kP = 0.4f;
		state->chassisState.rightPID.kI = 0.0f;
		state->chassisState.rightPID.kD = 0;
		state->chassisState.counter = 0;
		state->chassisState.leftSpeed = 0;
		state->chassisState.rightSpeed = 0;
		memory->isInitialized = True;
	}
}

ELON_CALLBACK(TempCallback){

	ELONState* state = scast<ELONState*>(memory->permanentStorage);
	ChassisState* chassisState = &(state->chassisState);
	ElevatorState* elevatorState = &(state->elevatorState);
	Gamepad* gamepad = GetGamepad(input, 0);

	F32 target = 3050;

	F32 leftSpeed = chassisState->dLeftEncoder / dt;
	F32 rightSpeed = chassisState->dRightEncoder / dt;
	chassisState->leftSpeed += leftSpeed;
	chassisState->rightSpeed += rightSpeed;
		

	F32 pidOutputLeft = PIDCalculation(memory, chassisState->leftEncoder, target, 
			dt, &chassisState->leftPID);

	F32 pidOutputRight = PIDCalculation(memory, chassisState->rightEncoder, target, 
			dt, &chassisState->rightPID);
#if 0
	if(chassisState->counter > 1.0f/dt){
		memory->Cout("%.02f ||| %.02f ||| %.02f ||| %.02f", chassisState->leftSpeed
					 * dt, chassisState->rightSpeed * dt, pidOutputLeft - target, 
					 pidOutputRight - target);
		chassisState->counter = 0;
		chassisState->leftSpeed = 0;
		chassisState->rightSpeed = 0;
	}
	chassisState->counter++;
#endif


	F32 normalizedOutputLeft = memory->Clamp(pidOutputLeft / target, -0.3f, 0.3f);
	F32 normalizedOutputRight = memory->Clamp(pidOutputRight / target, -0.3f, 0.3f);
#if 1
	F32 finalLeftOutput = 0;
	F32 finalRightOutput = 0;
	if (normalizedOutputLeft < 0 ){
		finalLeftOutput = normalizedOutputLeft-0.1329f;
	}else{
		finalLeftOutput = normalizedOutputLeft+0.0829f;
	}

	if (normalizedOutputRight < 0 ){
		finalRightOutput = normalizedOutputRight-0.089836f;
	}else{
		finalRightOutput = normalizedOutputRight;
	}

	//TankDrive(memory, finalLeftOutput, finalRightOutput);
#endif
	//TankDrive(memory, 0.0f, 0.0f);
#if 1
	memory->Cout("%f ||| %f ||| %d ||| %d ||| %f", normalizedOutputLeft, 
				 normalizedOutputRight,
				 chassisState->leftEncoder, chassisState->rightEncoder, 
				 (chassisState->leftEncoder + chassisState->rightEncoder)/2.0f);
#endif
}
	
}


#endif