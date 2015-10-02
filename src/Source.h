#ifndef ELON_SOURCE_H
#define ELON_SOURCE_H

#ifdef __cplusplus
extern "C" {
#endif

//TODO: Make this one type called ELON_CALLBACK

/**
 * Called at 50Hz when in teleop mode
 */
#define ELON_CALLBACK(name) void name(ELONMemory* memory)
typedef ELON_CALLBACK(ELONCallback);

#ifdef __cplusplus
}
#endif

#endif
