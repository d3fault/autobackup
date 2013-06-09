#!/bin/bash
read -n1 -r -p "make sure the device is mounted in: /media/disk/" key
cd /media/disk/DCIM/200MOVIE/

epochTime=`date +%s`
newDir=syncAt`echo $epochTime`
new750x1=/media/truecrypt1/polaroidVideo/`echo $newDir`/
new750x2=/media/truecrypt2/polaroidVideo/`echo $newDir`/

mkdir `echo $new750x1`
mkdir `echo $new750x2`

cp -avi ./ "$new750x1"
cp -avi ./ "$new750x2"

read -n1 -r -p "done copying, press any key to continue with delete" key

#shred on flash media does dick.
#sudo shred -fuv --iterations=1 ./*

cd ..
#gotta cd up and delete the folder in order to get hidden files. not really relevant on this polaroid script however...
rm -rfv ./200MOVIE/
#cool. simple file undelete utilities can now recover a video of me cumming on my own face :)
