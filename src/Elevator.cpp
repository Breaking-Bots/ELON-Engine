/*
 * Elevator.cpp
 *
 *  Created on: Sep 13, 2015
 *      Author: Zaeem
 */

#include "WPILib.h"
#include "Elevator.h"

SpeedController* motor; //Motor controller controlling elevator
F32 motorValue; //Motor speed values
I8 invertedMotor; //Inverted motor modifier

F32 elevatorMagnitude;

void InitializeElevator(U32 elevatorPort){
	motor = new Victor(elevatorPort);
	invertedMotor = 1;
	motor->Set(0.0f);
}

void TerminateElevator(){
	if(motor){
		delete motor;
	}
}

void Elevate(F32 speed){
	motorValue = Clamp(speed * invertedMotor, -1.0f, 1.0f);
}

void SetElevatorMagnitude(F32 magnitude){
	elevatorMagnitude = magnitude;
}

void InvertElevator(){
	invertedMotor *= -1.0f;
}

void UpdateElevator(){
	motor->Set(motorValue * elevatorMagnitude);
}
