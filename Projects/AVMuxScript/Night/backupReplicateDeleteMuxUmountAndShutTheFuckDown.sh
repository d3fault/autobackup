#!/bin/bash
exit 1

#config

videoSourceMountPoint=/mnt/videoSource
videoSource=$videoSourceMountPoint/goOutsideVids

audioSourceMountPoint=/mnt/audioSource
audioSource=$audioSourceMountPoint/VOICE/FOLDER01

backupMountPoint=/mnt/sdb
backupDir=$backupMountPoint/binary/outside/masterAV



#mount (some stay mounted, some we're just testing)

function mountAll()
{
	mount -v "$videoSourceMountPoint" || exit 1
	mount -v "$audioSourceMountPoint" || exit 1
	mount -v "$backupMountPoint" || exit 1
			
}
function umountAll()
{
	umount -v "$videoSourceMountPoint"
	umount -v "$audioSourceMountPoint"
	umount -v "$backupMountPoint"
}

mountAllStdOut=$(mountAll)
mountAllExitCode=$?
echo "$mountAllStdOut"
if [ $mountAllExitCode -ne 0 ]; then
	umountAllStdOut=$(umountAll)
	echo "$umountAllStdOut"
	echo "failed to mount all. check above for details"
	exit 1
fi
#every drive mounted, woot
if [ ! -d "$videoSource" ]; then
	umountAllStdOut=$(umountAll)
	echo "$umountAllStdOut"
	echo "video source does not exist: $videoSource"
	exit 1
fi
if [ ! -d "$audioSource" ]; then
	umountAllStdOut=$(umountAll)
	echo "$umountAllStdOut"
	echo "audio source does not exist: $videoSource"
	exit 1
fi

#backup

sudo mount "$backupMountPoint" || exit 1
if [ ! -d "$backupDir" ]; then
	echo "backup dir does not exist: $backupDir"
	umount "$backupMountPoint" #wrryyy i want QScopedPointers fuckers
	exit 1
fi
newDirInBackupDir=$backupDir/syncAt`date +%s`
if [ -d "$newDirInBackupDir" ]; then
	echo "dir we wanted to make and backup into already exists: $newDirInBackupDir"
	umount "$backupMountPoint"
	exit 1
fi
mkdir "$newDirInBackupDir"
if [ $? -ne 0 ]; then
	echo "failed to make dir: $newDirInBackupDir"
	umount "$backupMountPoint"
	exit 1
fi


#replicate

#delete source

#umount source

#mux

#umount

#shut the fuck down