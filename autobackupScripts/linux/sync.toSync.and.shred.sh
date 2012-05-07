#!/bin/sh
read -n1 -r -p "make sure the device is mounted in: /media/PENDRIVE/updates/toSync/" key
cd /media/PENDRIVE/updates/toSync/

epochTime=`date +%s`
newDir=syncAt`echo $epochTime`
new750x1=/mnt/750x1/onlineSyncs/`echo $newDir`/
new750x2=/mnt/750x2/onlineSyncs/`echo $newDir`/

sudo mkdir `echo $new750x1`
sudo mkdir `echo $new750x2`

sudo cp -av ./ "$new750x1"
sudo cp -av ./ "$new750x2"

read -n1 -r -p "done copying, press any key to begin shredding" key

find -type f -execdir sudo shred -fuv --iterations=1 '{}' \;
sudo rm -rfv ./*
