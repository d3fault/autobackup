#!/bin/bash
read -n1 -r -p "make sure the device is mounted in: /media/FC30-3DA9/" key
cd /media/FC30-3DA9/STEREO/

epochTime=`date +%s`
newDir=syncAt`echo $epochTime`
new750x1=/mnt/750x1/zoom/`echo $newDir`/
new750x2=/mnt/750x2/zoom/`echo $newDir`/

sudo mkdir `echo $new750x1`
sudo mkdir `echo $new750x2`

sudo cp -av ./ "$new750x1"
sudo cp -av ./ "$new750x2"

read -n1 -r -p "done copying, press any key to continue with shred. only shredding FOLDER01 so you gotta do any others manually" key

cd ./FOLDER01/

sudo shred -fuv --iterations=1 ./*
