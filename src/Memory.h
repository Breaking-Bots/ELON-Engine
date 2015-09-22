#ifndef ELON_MEMORY_H
#define ELON_MEMORY_H

#include "Util.h"

struct ELONMemory{
	B32 isInitialized;
	U32 permanentStorageSize;
	void* permanentStorage; //REQUIRED to be cleared to zero at startup
	U32 transientStorageSize;
	void* transientStorage; //REQUIRED to be cleared to zero at startup
};


struct ELONState{
	F32 chassisMagnitude;
	F32 elevatorMagnitude;
};


#endif
