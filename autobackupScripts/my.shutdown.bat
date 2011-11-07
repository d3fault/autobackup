@echo off
echo Syncing with 750gb drives...
call git.add.commit.and.push.bat
echo ...750gb drives synced
echo Dismounting 3x 750gb drives...
call dismount.3x.750gb.drives.bat
echo ...3x 750gb drives dismounted
echo Shutting Down
shutdown /s /t 3