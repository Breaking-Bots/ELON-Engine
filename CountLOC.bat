@echo off
ECHO Filename , LOC > LinesOfCode.csv
REM The following line sets delayed expansion, which is used to make sure variables
REM have real dynamic value.

SETLOCAL ENABLEDELAYEDEXPANSION
SET TotalLOC = 0
SET DIR=src

for /f "tokens=*" %%i in ('dir /b /a-d %DIR%') do (
set /a numLines = 0
for /f "tokens=*" %%j in (%DIR%\%%i) do (
set /a numLines=!numLines!+1
)
SET /a TotalLOC = TotalLOC + !numLines! >nul
echo %%i , !numLines! >> LinesOfCode.csv
)
echo Grand Total , %TotalLOC%  >> LinesOfCode.csv
echo Total Lines of Code %TotalLOC%
TIMEOUT 10
exit