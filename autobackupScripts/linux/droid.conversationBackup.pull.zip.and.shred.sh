#!/bin/bash
read -n1 -r -p "make sure the device is mounted in: /media/FF64-0C05/" key
cd /media/FF64-0C05/Android/data/net.ugorji.android.conversationbackup/files/
cp -av ./*.zip /mnt/750x1/droid/conversationBackup/
cp -av ./*.zip /mnt/750x2/droid/conversationBackup/
read -n1 -r -p "done copying, press a key to start shredding" key
shred -fuv --iterations=1 ./*.zip
