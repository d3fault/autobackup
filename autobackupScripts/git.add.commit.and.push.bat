@echo off
C:
chdir C:\cygwin\bin
bash --login -i -c "cd /cygdrive/c/autobackup/autobackupScripts/; sh ./git.cygwin.commands.sh; exit;"
echo added, commited, and pushed