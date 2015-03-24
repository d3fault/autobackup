@echo off
C:
chdir C:\cygwin\bin
echo make sure you set the correct drive path for the zoom in zoom.cygwin.commands.sh, press enter to continue
pause
bash --login -i -c "cd /cygdrive/c/autobackup/autobackupScripts/; sh ./zoom.cygwin.commands.sh; exit;"
echo copied all files from zoom. still need to be shredded but removable is read-only atm
pause