#!/bin/bash
targetDir=/root/deskVids
if [ ! -d "$targetDir" ]; then
	echo "dir doesn't exist. did you mount it?"
	exit 1
fi
unixTimeAt2015=`date --date="Thu Jan 1 00:00:00 MST 2015" +%s`
epochTime=`date +%s`
if [ "$epochTime" -lt "$unixTimeAt2015" ]; then
	echo "you didn't set the time"
	exit 1
fi
targetFile=$targetDir/deskAt.$epochTime.h264
if [ -f "$targetFile" ]; then
	echo "file already exists! you sure you set the date right? maybe there was a leap second"
	exit 1
fi
LD_LIBRARY_PATH=/opt/vc/lib/ /opt/vc/bin/raspivid -t 0 -o "$targetFile" --width 720 --height 480 --bitrate 300000 --vflip --framerate 16
