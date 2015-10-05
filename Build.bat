@echo off

IF NOT EXIST bin mkdir bin
IF NOT EXIST temp mkdir temp
IF NOT EXIST temp\\ELONEngine mkdir temp\\ELONEngine
IF NOT EXIST temp\\ELONHardwareLayer mkdir temp\\ELONHardwareLayer

pushd temp\\ELONEngine

set CommonCompilerFlags= -std=c++1y "-IC:\\Users\\Zaeem/wpilib/cpp/current/include" "-IZ:\\FRC\\Repositories\\ELON-Engine\\inc" -O0 -g3 -Wall
set CommonLinkerFlags= -shared-libgcc -fPIC

set ELONEngineSourceFiles= "..\\..\\src\\Util.cpp" "..\\..\\src\\Memory.cpp" "..\\..\\src\\Input.cpp" "..\\..\\src\\Chassis.cpp" "..\\..\\src\\Elevator.cpp" "..\\..\\src\\Actions.cpp" "..\\..\\src\\Source.cpp"
echo Compiling libELON.so
arm-frc-linux-gnueabi-g++ -shared %CommonCompilerFlags% %CommonLinkerFlags% -fmessage-length=0 -o ..\\..\\bin\\libELON.so -Wl,-Map,libELON.map %ELONEngineSourceFiles% 

popd
pushd temp\\ELONHardwareLayer

arm-frc-linux-gnueabi-g++ %CommonCompilerFlags% -c -fmessage-length=0 "..\\..\\src\\EHL.cpp"
echo Compiling FRCUserProgram
arm-frc-linux-gnueabi-g++ "-LC:\\Users\\Zaeem/wpilib/cpp/current/lib" -Wl,-rpath,/opt/GenICam_v2_3/bin/Linux_armv7-a -o "..\\..\\bin\\FRCUserProgram"  -Wl,-Map,EHL.map "EHL.o" -lwpi 

popd

