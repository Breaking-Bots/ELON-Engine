#pragma once

#include "Util.h"

/**
 * Initialize the thread space
 */
void InitializeThreadSpace();

/**
 * Terminates the thread space
 */
void TerminateThreadSpace();

/**
 * Checks if fast thread has started
 * Thread safe
 */
B32 IsFastThreadStarted();

/**
 * Checks if fast thread is running
 * Thread safe
 */
B32 IsFastThreadRunning();

/**
 * Pauses fast thread
 * Thread safe
 */
void PauseFastThread();

/**
 * Resumes fast thread
 * Thread safe
 */
void ResumeFastThread();

/**
 * Starts fast thread
 * Thread safe
 */
void StartFastThread();

/**
 * Stops fast thread
 * Thread safe
 */
void StopFastThread();

/**
 * Function callback for running the fast thread runtime on a separate thread
 */
I32 FastThreadRuntime(U32 targetHz);

/**
 * Runtime for core thread, runs slower than the fast thread
 * runnerCallback is the function provided that returns a B32 that determines whether the thread should run
 * executableCallback is the function provided that has the code that the user requires to be run at the
 * frequency provided
 */
I32 CoreThreadRuntime(U32 targetHz, B32_FUNCPTR runnerCallback, EXE_FUNCPTR executableCallback);

