#!/bin/bash

DIR_TO_SYNC=/media/disk/DCIM/200MOVIE
DIR1_TO_SYNC_TO=/media/truecrypt1/polaroidVideo
DIR2_TO_SYNC_TO=/media/truecrypt2/polaroidVideo

read -n1 -r -p "make sure the device is mounted in: $DIR_TO_SYNC -- and that both backup drives are mounted and writeable. We will only sync the videos, not pictures" key

if [ -d "$DIR_TO_SYNC" ]; then
	if [ -d "$DIR1_TO_SYNC_TO" ]; then
		if [ -d "$DIR2_TO_SYNC_TO" ]; then
			cd "$DIR_TO_SYNC";
			epochTime=`date +%s`
			newDir=syncAt`echo $epochTime`
			new750x1=`echo $DIR1_TO_SYNC_TO`/`echo $newDir`/
			new750x2=`echo $DIR2_TO_SYNC_TO`/`echo $newDir`/

			mkdir `echo $new750x1`
			mkdir `echo $new750x2`

			cp -avi ./ "$new750x1"
			cp -avi ./ "$new750x2"

			read -n1 -r -p "done copying, press any key to begin deleting (now is a good time to 'sync')" key

			rm -rfv ./*

			echo "Done syncing Polaroid Video Recorder";
		else
			echo "$DIR2_TO_SYNC_TO does not exist. Is it mounted?";
		fi
	else
		echo "$DIR1_TO_SYNC_TO does not exist. Is it mounted?";
	fi
else
	echo "$DIR_TO_SYNC does not exist. Is the device plugged in and mounted?";
fi
