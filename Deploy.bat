@echo off

ssh admin@roboRIO-5428.local "rm /home/lvuser/FRCUserProgram || true"
sftp -oBatchMode=no -b sftpbatchfile admin@roboRIO-5428.local
ssh admin@roboRIO-5428.local "killall netconsole-host"
sftp -oBatchMode=no -b sftpnetconsole admin@roboRIO-5428.local
ssh admin@roboRIO-5428.local ". /etc/profile.d/natinst-path.sh; chmod a+x /home/lvuser/FRCUserProgram; /usr/local/frc/bin/frcKillRobot.sh -t -r"
PAUSE