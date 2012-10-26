#!/bin/bash
read -n1 -r -p "make sure the device is mounted in: /media/disk/" key
cd /media/disk/STEREO/

epochTime=`date +%s`
newDir=syncAt`echo $epochTime`
new750x1=/media/truecrypt1/zoom/`echo $newDir`/
new750x2=/media/truecrypt2/zoom/`echo $newDir`/

mkdir `echo $new750x1`
mkdir `echo $new750x2`

cp -avi ./ "$new750x1"
cp -avi ./ "$new750x2"

read -n1 -r -p "done copying, press any key to continue with delete." key

cd ..
rm -rfv ./STEREO/

#cool, now simple file undelete tools can recover my meaning of life recordings :-P
#sudo shred -fuv --iterations=1 ./*
