@echo off

IF NOT EXIST bin mkdir bin
IF NOT EXIST Debug mkdir Debug
IF NOT EXIST Debug\\src mkdir Debug\\src
pushd Debug\\src

set CommonCompilerFlags= -std=c++14 "-IC:\\Users\\Zaeem\\wpilib\\cpp\\current\\include" "-IZ:\\FRC\\Workspace\\ELON-Engine\\src" -O0 -g3 -Wall
set CommonLinkerFlags= -shared-libgcc -fPIC

set ELONEngineSourceFiles= "..\\..\\src\\Util.cpp" "..\\..\\src\\Memory.cpp" "..\\..\\src\\Input.cpp" "..\\..\\src\\Chassis.cpp" "..\\..\\src\\Elevator.cpp" "..\\..\\src\\Actions.cpp" "..\\..\\src\\Source.cpp"

arm-frc-linux-gnueabi-g++ -shared %CommonCompilerFlags% %CommonLinkerFlags% -fmessage-length=0 -o ..\\..\\bin\\libELON.so %ELONEngineSourceFiles% 

arm-frc-linux-gnueabi-g++ %CommonCompilerFlags% -c -fmessage-length=0 "..\\..\\src\\EHL.cpp"

arm-frc-linux-gnueabi-g++ "-LC:\\Users\\Zaeem/wpilib/cpp/current/lib" -Wl,-rpath,/opt/GenICam_v2_3/bin/Linux_armv7-a %CommonLinkerFlags% -o ..\\..\\bin\\FRCUserProgram "EHL.o" -lwpi

popd
PAUSE

