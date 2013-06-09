#!/bin/bash
read -n1 -r -p "make sure the device is mounted in: /media/disk/" key
cd /media/disk/Android/data/net.ugorji.android.conversationbackup/files/
cp -avi ./*.zip /media/truecrypt1/droid/conversationBackup/
cp -avi ./*.zip /media/truecrypt2/droid/conversationBackup/
sync
read -n1 -r -p "done copying, press a key to start deleting." key
#shred -fuv --iterations=1 ./*.zip
cd ..
rm -rfv ./files/
