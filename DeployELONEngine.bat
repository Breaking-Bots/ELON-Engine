@echo off

IF NOT EXIST bin mkdir bin
IF NOT EXIST temp mkdir temp
IF NOT EXIST temp\\ELONEngine mkdir temp\\ELONEngine

pushd temp\\ELONEngine

set CommonCompilerFlags= -std=c++1y -fPIC -fext-numeric-literals "-IC:\\Users\\Zaeem/wpilib/cpp/current/include" "-IZ:\\FRC\\Repositories\\ELON-Engine\\inc" -O0 -g3 -w

echo Compiling libELON.so
arm-frc-linux-gnueabi-g++ -shared %CommonCompilerFlags% %CommonLinkerFlags% -fmessage-length=0 -o ..\\..\\bin\\libELON.so -Wl,-Map,libELON.map "..\\..\\src\\ELONEngine.cpp" 

popd

if errorlevel 0 (
	echo .
	echo ELONEngine Compilation Successful
	echo Deploying libELON.so
	ssh lvuser@roboRIO-5428.local "rm /home/lvuser/libELON.so || true;" 2>nul
	sftp -oBatchMode=no -b sftpELONEngine lvuser@roboRIO-5428.local
) else (
	echo .
   	echo ELONEngine Compilation Failed with status %errorlevel%
	echo Aborting Deployment
)

echo .