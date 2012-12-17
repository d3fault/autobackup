#!/bin/bash
read -n1 -r -p "make sure the device is mounted in: /media/disk/" key
cd /media/disk/DCIM/Camera/
cp -avi ./ /media/truecrypt1/droid/camera/
cp -avi ./ /media/truecrypt2/droid/camera/
cd ..
cd ..
cp -avi ./recording*.3gpp /media/truecrypt1/droidSoundRecorder/
cp -avi ./recording*.3gpp /media/truecrypt2/droidSoundRecorder/
read -n1 -r -p "done copying, press a key to start deleting" key
#shred -fuv --iterations=1 ./*
cd ./DCIM/
rm -rfv ./Camera/
rm -rfv ./.thumbnails/
#shred -fuv --iterations=1 ./*
cd ..
rm -rfv ./recording*.3gpp
