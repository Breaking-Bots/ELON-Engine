@echo off

CALL CountLOC.bat

IF "%~1"=="-u" (
	CALL DeployELONEngine.bat
	GOTO endofmake
)

CALL Build.bat

IF "%~1"=="-d" (
	CALL Deploy.bat
)

:endofmake
