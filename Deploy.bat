@echo off

ssh lvuser@roboRIO-5428.local "rm /home/lvuser/FRCUserProgram || true"
sftp -oBatchMode=no -b sftpbatchfile lvuser@roboRIO-5428.local
ssh lvuser@roboRIO-5428.local ". /etc/profile.d/natinst-path.sh; chmod a+x /home/lvuser/FRCUserProgram; /usr/local/frc/bin/frcKillRobot.sh -t -r"
PAUSE