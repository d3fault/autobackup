#!/bin/bash

DIR_TO_SYNC=/media/POLAROID8GB/DCIM/200MOVIE
DIR_TO_SYNC_TO=/home/d3fault/binarySupplement/Polaroid

read -n1 -r -p "Make sure the device is mounted in: $DIR_TO_SYNC and the place we will sync to is: $DIR_TO_SYNC_TO -- We will only sync the FOLDER01 folder" key

if [ ! -d "$DIR_TO_SYNC" ]; then
	echo "$DIR_TO_SYNC does not exist. Is the device plugged in and mounted?";
	exit 1
fi
if [ ! -d "$DIR_TO_SYNC_TO" ]; then
	echo "$DIR_TO_SYNC_TO does not exist. Did you clone it etc first?"
	exit 1
fi

cd "$DIR_TO_SYNC_TO"
epochTime=`date +%s`
newDir="$DIR_TO_SYNC_TO/syncAt$epochTime/"
mkdir "$newDir"
cd "$DIR_TO_SYNC"
cp -avi ./ "$newDir"
read -n1 -r -p "done copying, press any key to begin deleting (now is a good time to 'sync')" key
rm -rfv ./*
echo "Done syncing Polaroid Video Camera"
