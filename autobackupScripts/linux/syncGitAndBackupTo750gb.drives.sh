#!/bin/sh
cd ~/autobackup/
tree -DXs --timefmt %s -o dirstructure.txt
git add .
git commit -a -m "Auto-Commit @ `date`"
git push /mnt/750x1/autobackup/ master
git push /mnt/750x2/autobackup/ master

