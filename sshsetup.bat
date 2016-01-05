@echo off

ssh-keygen -t rsa
ssh admin@roboRIO-5428-FRC.local "mkdir ~/.ssh; chmod 777 ~/.ssh"
scp id_rsa.pub admin@roboRIO-5428-FRC.local:~/.ssh/authorized-keys
PAUSE