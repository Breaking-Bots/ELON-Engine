#ifndef TEMP_CPP
#define TEMP_CPP

extern "C"{

ELON_CALLBACK(InitTemp){
	if(!memory->isInitialized){
		ELONState* state = scast<ELONState*>(memory->permanentStorage);
		state->chassisState.chassisMagnitude = DEF_SPEED;
		state->elevatorState.elevatorMagnitude = DEF_SPEED;
		memory->isInitialized = True;
	}
}

ELON_CALLBACK(TempCallback){

	ELONState* state = scast<ELONState*>(memory->permanentStorage);
	Gamepad* gamepad = GetGamepad(input, 0);

	
}
	
}


#endif