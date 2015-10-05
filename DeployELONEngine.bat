@echo off

IF NOT EXIST bin mkdir bin
IF NOT EXIST temp mkdir temp
IF NOT EXIST temp\\ELONEngine mkdir temp\\ELONEngine

pushd temp\\ELONEngine

set CommonCompilerFlags= -std=c++1y "-IC:\\Users\\Zaeem/wpilib/cpp/current/include" "-IZ:\\FRC\\Repositories\\ELON-Engine\\inc" -O0 -g3 -Wall
set CommonLinkerFlags= -shared-libgcc -fPIC

set ELONEngineSourceFiles= "..\\..\\src\\Util.cpp" "..\\..\\src\\Memory.cpp" "..\\..\\src\\Input.cpp" "..\\..\\src\\Chassis.cpp" "..\\..\\src\\Elevator.cpp" "..\\..\\src\\Actions.cpp" "..\\..\\src\\Source.cpp"

echo Compiling libELON.so
arm-frc-linux-gnueabi-g++ -shared %CommonCompilerFlags% %CommonLinkerFlags% -fmessage-length=0 -o ..\\..\\bin\\libELON.so -Wl,-Map,libELON.map %ELONEngineSourceFiles% 

popd

echo Deploying libELON.so
REM ssh lvuser@roboRIO-5428.local "rm /home/lvuser/libELON.so || true;"
sftp -oBatchMode=no -b sftpELONEngine lvuser@roboRIO-5428.local
