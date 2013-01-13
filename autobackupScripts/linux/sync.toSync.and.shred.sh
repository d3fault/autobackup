#!/bin/bash
read -n1 -r -p "make sure the device is mounted in: /media/PENDRIVE/toSync/" key
cd /media/PENDRIVE/toSync/

epochTime=`date +%s`
newDir=syncAt`echo $epochTime`
new750x1=/media/truecrypt1/onlineSyncs/`echo $newDir`/
new750x2=/media/truecrypt2/onlineSyncs/`echo $newDir`/

mkdir `echo $new750x1`
mkdir `echo $new750x2`

cp -avi ./ "$new750x1"
cp -avi ./ "$new750x2"

read -n1 -r -p "done copying, press any key to begin deleting" key

#find -type f -execdir shred -fuv --iterations=1 '{}' \;

cd ..
rm -rfv ./toSync/
mkdir ./toSync/
