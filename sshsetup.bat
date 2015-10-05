@echo off

ssh-keygen -t rsa
ssh lvuser@roboRIO-5428.local mkdir -p .ssh
cat .ssh/id_rsa.pub | ssh lvuser@roboRIO-5428.local 'cat >> .ssh/authorized_keys'
PAUSE