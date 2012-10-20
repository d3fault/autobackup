#!/bin/bash
/usr/bin/truecrypt --text -v -p "`cat /home/d3fault/autobackup/autobackupScripts/linux/750x1`" --non-interactive --mount /dev/sdb1 /mnt/750x1
/usr/bin/truecrypt --text -v -p "`cat /home/d3fault/autobackup/autobackupScripts/linux/750x2`" --non-interactive --mount /dev/sdc1 /mnt/750x2
