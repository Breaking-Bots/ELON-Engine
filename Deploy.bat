@echo off

echo Removing Old Executables
ssh lvuser@roboRIO-5428.local "rm /home/lvuser/FRCUserProgram || true; rm /home/lvuser/libELON.so || true;"
echo Copy New Executables Over
sftp -oBatchMode=no -b sftpbatchfile lvuser@roboRIO-5428.local
echo Kill Netconsole
ssh admin@roboRIO-5428.local "killall netconsole-host" 2> nul
echo Start FRCUserProgram
ssh lvuser@roboRIO-5428.local ". /etc/profile.d/natinst-path.sh; chmod -R 777 /home/lvuser; chmod a+x /home/lvuser/FRCUserProgram; /usr/local/frc/bin/frcKillRobot.sh -t -r" 2> nul
