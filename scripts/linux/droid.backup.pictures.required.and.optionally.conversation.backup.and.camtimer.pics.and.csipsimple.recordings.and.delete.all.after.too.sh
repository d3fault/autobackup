#!/bin/bash

DIR_TO_SYNC=/media/disk/DCIM/Camera
DIR1_TO_SYNC_TO=/media/truecrypt1/droid/camera
DIR2_TO_SYNC_TO=/media/truecrypt2/droid/camera

read -n1 -r -p "make sure the device is mounted in: $DIR_TO_SYNC -- and that both backup drives are mounted and writeable" key

if [ -d "$DIR_TO_SYNC" ]; then
	if [ -d "$DIR1_TO_SYNC_TO" ]; then
		if [ -d "$DIR2_TO_SYNC_TO" ]; then
			cd "$DIR_TO_SYNC";

			cp -avi ./ "$DIR1_TO_SYNC_TO"
			cp -avi ./ "$DIR2_TO_SYNC_TO"

			read -n1 -r -p "done copying droid pictures, press any key to begin deleting (now is a good time to 'sync')" key

			rm -rfv ./*
			cd ..
			rm -rfv ./.thumbnails/

			echo "Done syncing Droid Pictures folder";
		else
			echo "$DIR2_TO_SYNC_TO does not exist. Is it mounted?";
		fi
	else
		echo "$DIR1_TO_SYNC_TO does not exist. Is it mounted?";
	fi

	#conversation backup
	CONV_BACKUP_SRC_FOLDER=/media/disk/Android/data/net.ugorji.android.conversationbackup/files
	if [ -d "$CONV_BACKUP_SRC_FOLDER" ]; then
		CONV_BACKUP_DEST_FOLDER1=/media/truecrypt1/droid/conversationBackup
		if [ -d "$CONV_BACKUP_DEST_FOLDER1" ]; then
			CONV_BACKUP_DEST_FOLDER2=/media/truecrypt2/droid/conversationBackup
			if [ -d "$CONV_BACKUP_DEST_FOLDER2" ]; then
				cd "$CONV_BACKUP_SRC_FOLDER";

				cp -avi ./*.zip "$CONV_BACKUP_DEST_FOLDER1"
				cp -avi ./*.zip "$CONV_BACKUP_DEST_FOLDER2"

				read -n1 -r -p "done copying droid conversation backup zips, press any key to continue deleting (now would be a good time to 'sync')" key

				rm -rfv ./*.zip

				echo "Done syncing Droid Conversation Backup";
			else
				echo "Destination Folder1 for conversation backup: $CONV_BACKUP_DEST_FOLDER2 does not exist, but the source does. Make it and re-run this script";
			fi
		else
			echo "Destination Folder1 for conversation backup: $CONV_BACKUP_DEST_FOLDER1 does not exist, but the source does. Make it and re-run this script";
		fi
	else
		echo "Conversation Backup folder does not exist on drive, skipping since optional";
	fi

	#camtimer pics
	CAMTIMER_SRC_FOLDER=/media/disk/CamTimer
	if [ -d "$CAMTIMER_SRC_FOLDER" ]; then
		CAMTIMER_DEST_FOLDER1=/media/truecrypt1/droid/camera/camtimer
		if [ -d "$CAMTIMER_DEST_FOLDER1" ]; then
			CAMTIMER_DEST_FOLDER2=/media/truecrypt2/droid/camera/camtimer
			if [ -d "$CAMTIMER_DEST_FOLDER2" ]; then
				cd "$CAMTIMER_SRC_FOLDER";

				cp -avi ./ "$CAMTIMER_DEST_FOLDER1"
				cp -avi ./ "$CAMTIMER_DEST_FOLDER2"

				read -n1 -r -p "done copying droid cam timer pics, press any key to continue deleting (now is a good time to 'sync')" key

				rm -rfv ./*

				echo "Done syncing droid cam timer pics";
			else
				echo "Destination Folder2 for camtimer does not exist, but the source does. Make it and re-run this script";
			fi
		else
			echo "Destination Folder1 for camtimer does not exist, but the source does. Make it re-run this script";
		fi
	else
		echo "Camtimer folder does not exist on drive, skipping since optional";
	fi

	#csipsimple recordings
	CSIPSIMPLE_RECORDINGS_FOLDER=/media/disk/CSipSimple/records
	if [ -d "$CSIPSIMPLE_RECORDINGS_FOLDER" ]; then
		CSIPSIMPLE_DEST_FOLDER1=/media/truecrypt1/droid/CSipSimpleRecordings
		if [ -d "$CSIPSIMPLE_DEST_FOLDER1" ]; then
			CSIPSIMPLE_DEST_FOLDER2=/media/truecrypt2/droid/CSipSimpleRecordings
			if [ -d "$CSIPSIMPLE_DEST_FOLDER2" ]; then
				cd "$CSIPSIMPLE_RECORDINGS_FOLDER";

				cp -avi ./ "$CSIPSIMPLE_DEST_FOLDER1"
				cp -avi ./ "$CSIPSIMPLE_DEST_FOLDER2"

				read -n1 -r -p "done copying droid csipsimple recordings, press any key to continue deleting (now would be a good time to 'sync')" key

				rm -rfv ./*

				echo "Done syncing droid CSipSimple Recordings";
			else
				echo "Destinationi Folder2 for CSipSimple recordings does not exist, but the source does. Make it and re-run this script";
			fi
		else
			echo "Destination Folder1 for CSipSimple recordings does not exist, but the source does. Make it and re-run this script";
		fi
	else
		echo "CSipSimple recordings folder does not exist. Skipping since optional";
	fi


	#voice recordings
	#todo

	echo "Droid Backup Script completely finished";
else
	echo "$DIR_TO_SYNC does not exist. Is the device plugged in and mounted?";
fi
