#!/bin/bash
##asumes qt/git/7za/tmux/boost-sid/wt-compiled, in addition to binary placed in /home/user/hvbs/bin/ on remote server##
exit 1

#if changing these variables, paste the changes into postLaunch
userHome=/home/user
hvbs=$userHome/hvbs
hvbsWebSrc=$hvbs/webSrc
oldUnversionedArchiveWebSrcTemp=$hvbsWebSrc/oldUnversionedArchiveTemp
semiOldSemiUnversionedWebSrcTemp=$hvbsWebSrc/semiOldSemiUnversionedArchiveTemp
autobackupWebSrcTemp=$hvbsWebSrc/autobackupTemp

hvbsConfigDir=$userHome/.config/HackyVideoBullshitSiteOrganization
hvbsConfigFile=$hvbsConfigDir/HackyVideoBullshitSite.ini

mkdir -p $oldUnversionedArchiveWebSrcTemp/empty0/view
ln -s $oldUnversionedArchiveWebSrcTemp/empty0/view $oldUnversionedArchiveWebSrcTemp/empty0/download
ln -s $oldUnversionedArchiveWebSrcTemp/empty0 $oldUnversionedArchiveWebSrcTemp/oldUnversionedArchiveCurrentSymlink

mkdir -p $semiOldSemiUnversionedWebSrcTemp/empty0/view
ln -s $semiOldSemiUnversionedWebSrcTemp/empty0/view $semiOldSemiUnversionedWebSrcTemp/empty0/download
ln -s $semiOldSemiUnversionedWebSrcTemp/empty0 $semiOldSemiUnversionedWebSrcTemp/semiOldSemiUnversionedArchiveCurrentSymlink

mkdir -p $autobackupWebSrcTemp/empty0/view
ln -s $autobackupWebSrcTemp/empty0/view $autobackupWebSrcTemp/empty0/download
ln -s $autobackupWebSrcTemp/empty0 $autobackupWebSrcTemp/autobackupCurrentSymlink

derMessage="this folder will contain a lot more files in just a few minutes, please check back later"
echo "$derMessage" > $oldUnversionedArchiveWebSrcTemp/empty0/view/readme.txt
echo "$derMessage" > $semiOldSemiUnversionedWebSrcTemp/empty0/view/readme.txt
echo "$derMessage" > $autobackupWebSrcTemp/empty0/view/readme.txt

#sure i could have just used one readme.txt and last modified timestamps file and symlink'd them all together, but i'm confused enough as it is
derTiemstomp=`date +%s`
echo "readme.txt:$derTiemstomp" > $oldUnversionedArchiveWebSrcTemp/empty0/view/.lastModifiedTimestamps
echo "readme.txt:$derTiemstomp" > $semiOldSemiUnversionedWebSrcTemp/empty0/view/.lastModifiedTimestamps
echo "readme.txt:$derTiemstomp" > $autobackupWebSrcTemp/empty0/view/.lastModifiedTimestamps

hvbsView=$hvbs/web/view
hvbsDownload=$hvbs/web/download

mkdir -p $hvbsView
mkdir -p $hvbsDownload

ln -s $oldUnversionedArchiveWebSrcTemp/oldUnversionedArchiveCurrentSymlink/view $hvbsView/oldUnversionedArchive
ln -s $oldUnversionedArchiveWebSrcTemp/oldUnversionedArchiveCurrentSymlink/download $hvbsDownload/oldUnversionedArchive

ln -s $semiOldSemiUnversionedWebSrcTemp/semiOldSemiUnversionedArchiveCurrentSymlink/view $hvbsView/semiOldSemiUnversionedArchive
ln -s $semiOldSemiUnversionedWebSrcTemp/semiOldSemiUnversionedArchiveCurrentSymlink/download $hvbsDownload/semiOldSemiUnversionedArchive

ln -s $autobackupWebSrcTemp/autobackupCurrentSymlink/view $hvbsView/autobackupLatest
ln -s $autobackupWebSrcTemp/autobackupCurrentSymlink/download $hvbsDownload/autobackupLatest

#configure hvbs as non-propagating node
mkdir -p $hvbs/incomingVideoScratch/upload
mkdir -p $hvbs/incomingVideoScratch/watched
mkdir -p $hvbs/incomingVideoScratch/yearDayFolderSetup
mkdir -p $hvbs/web/view/AirborneVideos
mkdir -p $hvbsConfigDir
echo "[General]" > $hvbsConfigFile
echo "HvbsWebBaseDir=$hvbs/web" >> $hvbsConfigFile
echo "AirborneVideoSegmentsBaseDir_aka_videoSegmentsImporterFolderToMoveTo=$hvbs/web/view/AirborneVideos" >> $hvbsConfigFile
echo "NeighborPropagationRemoteDestinationToMoveTo=/dev/null" >> $hvbsConfigFile
echo "NeighborPropagationRemoteSftpUploadScratchSpace=/dev/null" >> $hvbsConfigFile
echo "NeighborPropagationUserHostPathComboSftpArg=null@dev.poopybutt" >> $hvbsConfigFile
echo "SftpProcessPath=/usr/bin/sftp" >> $hvbsConfigFile
echo "VideoSegmentsImporterFolderToWatch=$hvbs/incomingVideoScratch/watched" >> $hvbsConfigFile
echo "VideoSegmentsImporterFolderYearDayOfYearFoldersScratchSpace=$hvbs/incomingVideoScratch/yearDayFolderSetup" >> $hvbsConfigFile
echo "" >> $hvbsConfigFile
echo "[LastModifiedTimestampFiles]" >> $hvbsConfigFile
echo "1\AbsoluteFilePathOfLastModifiedTimestampsFile=$hvbsView/oldUnversionedArchive/.lastModifiedTimestamps" >> $hvbsConfigFile
echo "2\AbsoluteFilePathOfLastModifiedTimestampsFile=$hvbsView/semiOldSemiUnversionedArchive/.lastModifiedTimestamps" >> $hvbsConfigFile
echo "3\AbsoluteFilePathOfLastModifiedTimestampsFile=$hvbsView/autobackupLatest/.lastModifiedTimestamps" >> $hvbsConfigFile
echo "size=3" >> $hvbsConfigFile


cd $hvbs/bin
ln -s /usr/local/share/Wt/resources/ resources
echo "#!/bin/bash" > ./hvbsLauncher.sh
echo "LD_LIBRARY_PATH=/usr/local/lib ./HackyVideoBullshitSite --docroot \".;/resources\" --http-address 0.0.0.0 --http-port 7777" >> ./hvbsLauncher.sh
chmod u+x ./hvbsLauncher.sh
tmux new-session -d -s hvbs './hvbsLauncher.sh'
#./hvbsLauncher.sh
