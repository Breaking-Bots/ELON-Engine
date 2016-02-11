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
		state->chassisState.leftPID.kP = 0.0001f;
		state->chassisState.leftPID.kI = 0.2f;
		state->chassisState.leftPID.kD = -0.050f;
		state->chassisState.rightPID.kP = 0.4f;
		state->chassisState.rightPID.kI = 0.0f;
		state->chassisState.rightPID.kD = 0;
		state->chassisState.counter = 0;
		state->chassisState.leftSpeed = 0;
		state->chassisState.rightSpeed = 0;
		state->chassisState.vLeft = 0;
		state->chassisState.vRight = 0;
		state->chassisState.leftTotal = 0;
		state->chassisState.leftCount = 0;
		memory->isInitialized = True;
	}
}

ELON_CALLBACK(TempCallback){

	ELONState* state = scast<ELONState*>(memory->permanentStorage);
	ChassisState* chassisState = &(state->chassisState);
	ElevatorState* elevatorState = &(state->elevatorState);
	Gamepad* gamepad = GetGamepad(input, 0);

	S32 lb = (S32)Button(gamepad, _LB);
	S32 rb = (S32)Button(gamepad, _RB);
	S32 l3 = (S32)Button(gamepad, _L3);
	F32 lt = Analog(gamepad, _LT);
	F32 rt = Analog(gamepad, _RT);
	chassisState->motorValues[0] = -0.2f * (memory->Clamp(rb, -1.0f, 1.0f) * chassisState->invertedMotors[0]);
	chassisState->motorValues[1] = 0.8f * (memory->Clamp(lb, -1.0f, 1.0f) * chassisState->invertedMotors[1]);
	chassisState->motorValues[2] = 0.8f * (memory->Clamp(lb, -1.0f, 1.0f) * chassisState->invertedMotors[2]);
	chassisState->motorValues[3] = -0.2f * (memory->Clamp(rb, -1.0f, 1.0f) * chassisState->invertedMotors[3]);
	
	if(rt + lt > 0.0f){
		chassisState->motorValues[0] = 1.0f * (memory->Clamp(rt, -1.0f, 1.0f) * chassisState->invertedMotors[0]);
		chassisState->motorValues[1] = 1.0f * (memory->Clamp(lt, -1.0f, 1.0f) * chassisState->invertedMotors[1]);
		chassisState->motorValues[2] = 1.0f * (memory->Clamp(lt, -1.0f, 1.0f) * chassisState->invertedMotors[2]);
		chassisState->motorValues[3] = 1.0f * (memory->Clamp(rt, -1.0f, 1.0f) * chassisState->invertedMotors[3]);
	}

	if(l3){
		chassisState->motorValues[0] = -1.0f * chassisState->invertedMotors[0];
		chassisState->motorValues[1] = -1.0f * chassisState->invertedMotors[1];		
		chassisState->motorValues[2] = -1.0f * chassisState->invertedMotors[2];
		chassisState->motorValues[3] = -1.0f * chassisState->invertedMotors[3];	
	}

	if((S32)ButtonTapped(gamepad, _R3) && !state->started){
		state->startTime = 0;
		state->started = True;
	}

	U32 cycles = (U32)(0.1475f * CORE_THREAD_HZ);

	if(state->started){
		if(state->startTime < cycles){
			chassisState->motorValues[0] = -0.5f * 1.0f * chassisState->invertedMotors[0];
			chassisState->motorValues[1] = -0.5f * 1.0f * chassisState->invertedMotors[1];		
			chassisState->motorValues[2] = -0.5f * 1.0f * chassisState->invertedMotors[2];
			chassisState->motorValues[3] = -0.5f * 1.0f * chassisState->invertedMotors[3];			
		}else{
			chassisState->motorValues[0] = 0.0f;
			chassisState->motorValues[1] = 0.0f;	
			chassisState->motorValues[2] = 0.0f;
			chassisState->motorValues[3] = 0.0f;
			state->started = False;	
		}
		state->startTime++;
	}


#if 0
	F32 target = 75;

	F32 lSpeed = chassisState->dLeftEncoder;
	F32 rSpeed = chassisState->dRightEncoder;
	chassisState->leftSpeed = lSpeed;
	chassisState->leftTotal += lSpeed;
	chassisState->leftCount++;

	F32 pidOutputLeft = PIDCalculation(memory, lSpeed, target, 
			dt, &chassisState->leftPID);

	memory->Cout("%.06f ||| %.06f ||| %.06f", lSpeed, rSpeed, pidOutputLeft);

	//chassisState->vLeft += pidOutputLeft;
	//chassisState->vRight += pidOutputRight;

	//memory->Cout("lSpeed: %.02f vLeft: %.02f", chassisState->leftSpeed, chassisState->vLeft);
#if 1
	if(chassisState->counter > 20){
		//memory->Cout("Avg Speed Left: %.06f", vAvg);
		chassisState->leftAvg = chassisState->leftTotal/chassisState->leftCount;
		chassisState->counter = 0;
		chassisState->leftSpeed = 0;
		chassisState->rightSpeed = 0;
		chassisState->leftTotal = 0;
		chassisState->leftCount = 0;
	}
	chassisState->counter++;
#endif
	//temp
	F32 velocityFactor = 1.0f/310.0f;

	F32 normalizedOutputLeft = memory->Clamp((pidOutputLeft + target) * velocityFactor,
							   -1.0f, 1.0f);


#if 1
	F32 finalLeftOutput = normalizedOutputLeft;
	/*if (normalizedOutputLeft < 0 ){
		finalLeftOutput = normalizedOutputLeft-0.1329f;
	}else{
		finalLeftOutput = normalizedOutputLeft+0.0829f;
	}*/


	TankDrive(memory, finalLeftOutput, finalLeftOutput);
#else
	TankDrive(memory, 1.0f, 1.0f);
#endif
#if 0
	memory->Cout("%f ||| %f ||| %d ||| %d ||| %f", normalizedOutputLeft, 
				 normalizedOutputRight,
				 chassisState->leftEncoder, chassisState->rightEncoder, 
				 (chassisState->leftEncoder + chassisState->rightEncoder)/2.0f);
#endif
#endif
	
}
}


#endif