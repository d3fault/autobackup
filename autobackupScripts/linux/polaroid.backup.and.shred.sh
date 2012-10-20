#!/bin/bash
read -n1 -r -p "make sure the device is mounted in: /media/9CB5-7BF4/" key
cd /media/9CB5-7BF4/DCIM/200MOVIE/

epochTime=`date +%s`
newDir=syncAt`echo $epochTime`
new750x1=/mnt/750x1/polaroidVideo/`echo $newDir`/
new750x2=/mnt/750x2/polaroidVideo/`echo $newDir`/

sudo mkdir `echo $new750x1`
sudo mkdir `echo $new750x2`

sudo cp -av ./ "$new750x1"
sudo cp -av ./ "$new750x2"

read -n1 -r -p "done copying, press any key to continue with shred" key

sudo shred -fuv --iterations=1 ./*
