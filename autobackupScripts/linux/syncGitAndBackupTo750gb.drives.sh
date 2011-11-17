#!/bin/sh
cd ~/autobackup/
git add .
git commit -a -m "Auto-Commit @ `date`"
git push /mnt/750x1/autobackup/ master
git push /mnt/750x2/autobackup/ master

