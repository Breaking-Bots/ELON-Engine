@echo off
@setlocal

IF NOT EXIST bin mkdir bin
IF NOT EXIST temp mkdir temp
IF NOT EXIST temp\\ELONEngine mkdir temp\\ELONEngine
IF NOT EXIST temp\\ELONHardwareLayer mkdir temp\\ELONHardwareLayer

pushd temp\\ELONEngine

set CommonCompilerFlags= -std=c++1y -fext-numeric-literals "-IC:\\Users\\Zaeem/wpilib/cpp/current/include" "-IZ:\\FRC\\Repositories\\ELON-Engine\\inc" "-IZ:\\FRC\\Workspace\\ELON Engine Prototype\\inc" -O0 -g3 -w
set CommonLinkerFlags= -shared-libgcc -fPIC

arm-frc-linux-gnueabi-g++ -shared %CommonCompilerFlags% %CommonLinkerFlags% -fmessage-length=0 -o ..\\..\\bin\\libELON.so -Wl,-Map,libELON.map "..\\..\\src\\ELONEngine.cpp"

IF errorlevel 0 (
	echo .
	echo ELONEngine Successfully Compiled
) else (
	echo .
  	echo ELONEngine Compilation Failed with status %errorlevel%
   	exit /b %errorlevel%
)

popd
pushd temp\\ELONHardwareLayer

arm-frc-linux-gnueabi-g++ %CommonCompilerFlags% -c -fmessage-length=0 "..\\..\\src\\EHL.cpp"

echo .
IF errorlevel 0 (
	echo FRCUserProgram Successfully Compiled
) else (
   	echo FRCUserProgram Compilation Failed with status %errorlevel%
   	exit /b %errorlevel%
)

arm-frc-linux-gnueabi-g++ "-LC:\\Users\\Zaeem/wpilib/cpp/current/lib" -Wl,-rpath,/opt/GenICam_v2_3/bin/Linux_armv7-a -o "..\\..\\bin\\FRCUserProgram"  -Wl,-Map,EHL.map "EHL.o" -lwpi 

popd

exit /b 0

