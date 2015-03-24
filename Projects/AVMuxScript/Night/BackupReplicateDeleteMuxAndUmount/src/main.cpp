#include <QCoreApplication>

#include <QProcess>
#include <QDebug>
#include <QScopedPointer>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QDirIterator>
#include <QSharedPointer>
#include <QTemporaryDir>
#include <QTemporaryFile>


//MANDATORY config -- NOTE: do not use paths with spaces:
#define videoSourceMountPoint "/mnt/videoSource"
#define videoSource videoSourceMountPoint "/goOutsideVids"

#define audioSourceMountPoint "/mnt/audioSource"
#define audioSource audioSourceMountPoint "/VOICE/FOLDER01"

#define videoFps "16" /* this must be kept in sync with whatever the capture device is using. Don't change this value mid-day. only change it in between days (ie, at morning before recording, or at night _after_ running this script). I considered keeping it in sync with the value in DirectoriesOfAudioAndVideoFilesMuxerSyncer, but opted not to (my master copies will be 16fps, the web/muxed ones 10fps) */

#define destPathBinaryRoot "/binary"
#define outsidePathRelativeToMountPoint destPathBinaryRoot "/Videos/FirstPerson/Hat"
#define allSigsFileName "allSigs.txt" //.asc? it's custom so nah
#define sigsFilePathRelativeToMountPoint destPathBinaryRoot "/" allSigsFileName

#define backupMountPoint "/mnt/sdb"
#define backupDestOutsideBase backupMountPoint outsidePathRelativeToMountPoint

//NOTE: the _last_ replica in this list is used as the "air gap" drive (passed to morning script). no replicas are necessary, and no mux/sync'ing is done if none are available
#define allReplicas \
QList<QPair<QString /* replica mount point */, QString /* replica binary root */> > replicas; \
replicas << qMakePair(QString("/mnt/sdc"), QString("/mnt/sdc" + QString(destPathBinaryRoot)));
//replicas << qMakePair(QString("/mnt/blackCavalry"), QString("/mnt/blackCavalry" + QString(destPathBinaryRoot)));
#define airGapReplicaTempDirForMuxedCopiesRelativeToMountPoint "/temp/forMorningScript"

#define muxerSyncerBinaryFilePath "/home/d3fault/autobackup/Projects/DirectoriesOfAudioAndVideoFilesMuxerSyncer/build-DirectoriesOfAudioAndVideoFilesMuxerSyncerCli-Desktop_Qt_5_4_1_GCC_64bit-Release/DirectoriesOfAudioAndVideoFilesMuxerSyncerCli"
#define recursiveGpgSignerBinaryFilePath "/home/d3fault/autobackup/Projects/RecursiveGpgTools/RecursivelyGpgSignIntoCustomDetachedSignaturesFormat/src/build-RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli-Desktop_Qt_5_4_1_GCC_64bit-Release/RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli"

//optional config:
#define backupDestPrefix "/daySyncedAt-"
//#define backupDestNewFolderForThisSync backupDestBase backupDestPrefix
#define backupDestAudioSubDir "a"
#define backupDestVideoSubDir "v"

#if 0 //corresponding fstab parts (for reference)
/dev/sdb1 /mnt/sdb auto rw,user,noauto 0 0
/dev/sdc1 /mnt/sdc auto rw,user,noauto 0 0
UUID=5d147465-f356-4d87-90cd-260b03305147 /mnt/videoSource ext4 rw,user,noauto 0 0
LABEL=IC\040RECORDER /mnt/audioSource vfat rw,user,noauto 0 0
UUID=0A5C1C5C6C0566FD /mnt/blackCavalry auto rw,user,noauto 0 0
#endif


#define DUMP_PROCESS_OUTPUT(theProcess) \
qDebug() << theProcess.readAllStandardError(); \
qDebug() << theProcess.readAllStandardOutput();
static bool runProcess(const QString &cmd, const QString &workingDirectory_OrEmptyStringIfToNotSet = QString())
{
    qDebug() << "about to run process:" << cmd;
    QProcess process;
    if(!workingDirectory_OrEmptyStringIfToNotSet.isEmpty())
    {
        qDebug() << "...with working dir of:" << workingDirectory_OrEmptyStringIfToNotSet;
        process.setWorkingDirectory(workingDirectory_OrEmptyStringIfToNotSet);
    }
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.start(cmd);
    if(!process.waitForStarted(-1))
    {
        qDebug() << "failed to start:" << cmd;
        return false;
    }
    if(!process.waitForFinished(-1))
    {
        qDebug() << "failed to finish:" << cmd;
        return false;
    }
    if(process.exitCode() != 0 || process.exitStatus() != QProcess::NormalExit)
    {        
        qDebug() << "command exitted abnormally with exit code:" << process.exitCode() << " -- " << cmd;
        return false;
    }
    return true;
}
struct BeforeAndAfterProcessRunner //I was just gonna use these on the stack, but putting them in scoped pointers lets me speed up the destruction with ease (and the alternative was to add extra curly brace scopes lolol fuck that)
{
    BeforeAndAfterProcessRunner(const QString &cmdToRunAtConstruction, const QString &cmdToRunAtDestructionUnlessConstructionProcessFailed)
        : m_CmdToRunAtDestructionUnlessConstructionProcessFailed(cmdToRunAtDestructionUnlessConstructionProcessFailed)
    {
        CmdRunAtConstructionSucceeded = runProcess(cmdToRunAtConstruction);
    }
    ~BeforeAndAfterProcessRunner()
    {
        if(CmdRunAtConstructionSucceeded)
        {
            runProcess(m_CmdToRunAtDestructionUnlessConstructionProcessFailed);
        }
    }
    bool CmdRunAtConstructionSucceeded;
private:
    QString m_CmdToRunAtDestructionUnlessConstructionProcessFailed;
};
struct AutoRemovingDirMker
{
    AutoRemovingDirMker(const QString &dirToMk)
        : AutoRemove(true)
        , m_DirToMk(dirToMk)
    {
        QDir dirInstance(dirToMk);
        MkDirSucceeded = dirInstance.mkdir(dirToMk);
        if(!MkDirSucceeded)
        {
            qDebug() << "failed to mkdir:" << dirToMk;
        }
    }
    ~AutoRemovingDirMker()
    {
        if(AutoRemove && MkDirSucceeded)
        {
            QDir dirInstance(m_DirToMk);
            if(!dirInstance.rmdir(m_DirToMk))
            {
                qDebug() << "failed to rmdir:" << m_DirToMk;
            }
        }
    }
    bool MkDirSucceeded;
    bool AutoRemove;
private:
    const QString &m_DirToMk;
};

#define mountCmdPrefix "mount -v "
#define sudoMountCmdPrefix "sudo " mountCmdPrefix
#define umountCmdPrefix "umount -v "
#define sudoUmountCmdPrefix "sudo " umountCmdPrefix

//i had specified partial, but took it out in favor of backup (because I wouldn't want to backup a partial lol
#define rsyncCmdPrefix "rsync -avhh --progress --protect-args"
#define rsyncCmdExcludingAllSigsPrefix rsyncCmdPrefix " --exclude '/" allSigsFileName "' --backup --suffix=~accidentallyOverwrittenDuringSyncAt-"
#define rsyncCmdMiddle " ./ " //the src dir, but we are setting the process's working directory to the src dir instead (/path/to/src/* would miss hidden files)

#define RETURN_ONE_IF_CMD_RUN_AT_CONSTRUCTOR_OF_SMART_PROCESS_THINGO_FAILS(scopedPointerVarName, constructCmd, destructCmd) \
QScopedPointer<BeforeAndAfterProcessRunner> scopedPointerVarName(new BeforeAndAfterProcessRunner(constructCmd, destructCmd)); \
if(!scopedPointerVarName->CmdRunAtConstructionSucceeded) \
{ \
    qDebug() << "exitting because command failed: " << constructCmd; \
    return 1; \
}

#define RETURN_ONE_IF_CMD_RUN_AT_CONSTRUCTOR_OF_SMART_PROCESS_THINGO_FAILS_LIST_VARIANT(listToAppendTo, constructCmd, destructCmd) \
QSharedPointer<BeforeAndAfterProcessRunner> aSharedPointer(new BeforeAndAfterProcessRunner(constructCmd, destructCmd)); \
if(!aSharedPointer->CmdRunAtConstructionSucceeded) \
{ \
    return 1; \
} \
listToAppendTo << aSharedPointer;

#define RETURN_ONE_IF_DIR_IS_NOT_GOOD(dirToCheck) \
{ \
    QFileInfo dir(dirToCheck); \
    if(!dir.exists()) \
    { \
        qDebug() << "dir does not exist:" << dirToCheck; \
        return 1; \
    } \
    if(!dir.isDir()) \
    { \
        qDebug() << "dir is not really a dir:" << dirToCheck; \
        return 1; \
    } \
    if(!dir.isWritable()) \
    { \
        qDebug() << "dir not writable:" << dirToCheck; \
        return 1; \
    } \
}

#define RETURN_ONE_IF_RSYNC_COPY_CMD_FAILS(rsyncCmdSource, rsyncCmdDest) \
if(!runProcess(rsyncCmdExcludingAllSigsPrefix + unixTimeInSecondsNow + rsyncCmdMiddle + rsyncCmdDest, rsyncCmdSource)) \
{ \
    qDebug() << "failed to backup (" << rsyncCmdSource << ") to (" << rsyncCmdDest << ")"; \
    return 1; \
}

#define RETURN_ONE_IF_RECURSIVE_DELETE_OR_REMAKE_FAILS(dirToDeleteAndRemake) \
{ \
    QDir theDir(dirToDeleteAndRemake); \
    if(!theDir.removeRecursively()) \
    { \
        qDebug() << "failed to remove recursively:" << dirToDeleteAndRemake; \
        return 1; \
    } \
    if(!theDir.mkdir(dirToDeleteAndRemake)) \
    { \
        qDebug() << "failed to re-create dir just after recursively deleting it:" << dirToDeleteAndRemake; \
        return 1; \
    } \
}

#define RETURN_ONE_IF_MKDIR_AT_CONSTRUCTOR_OF_SMART_MKDIR_THINGO_FAILS(scopedPointerVarName, dirToMk) \
QScopedPointer<AutoRemovingDirMker> scopedPointerVarName(new AutoRemovingDirMker(dirToMk)); \
if(!scopedPointerVarName->MkDirSucceeded) \
{ \
    return 1; \
}

#define RETURN_ONE_IF_MKDIR_AT_CONSTRUCTOR_OF_SMART_MKDIR_THINGO_FAILS_LIST_VARIANT(replicaDirs, dirToMk) \
QSharedPointer<AutoRemovingDirMker> aSharedPointer(new AutoRemovingDirMker(dirToMk)); \
if(!aSharedPointer->MkDirSucceeded) \
{ \
    return 1; \
} \
replicaDirs << aSharedPointer;

#define RETURN_ONE_IF_VIDEOSOURCE_DIR_HAS_ANY_NONFILES_OR_NONH264FILES(dirToCheck) \
{ \
    QDirIterator it(dirToCheck, (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden), QDirIterator::Subdirectories); \
    while(it.hasNext()) \
    { \
        it.next(); \
        const QFileInfo &currentEntry = it.fileInfo(); \
        if(!currentEntry.isFile()) \
        { \
            qDebug() << "non-file found in:" << dirToCheck; \
            return 1; \
        } \
        if(currentEntry.suffix().toLower() != "h264") \
        { \
            qDebug() << "non .h264 file found in video source" << currentEntry.absoluteFilePath(); \
            return 1; \
        } \
    } \
}

#define RETURN_ONE_IF_ANY_FILE_IN_VIDEOSOURCE_FAILS_TO_COPYMUX(theVideoSourceDir, copyMuxToDestDir_WithSlashAppended, ffmpegTruncateArgOrZeroIfToNotSpecify) \
{ \
    QDirIterator videoSrcDirIterator(theVideoSourceDir, (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden)/*, QDirIterator::Subdirectories*/); \
    while(videoSrcDirIterator.hasNext()) \
    { \
        videoSrcDirIterator.next(); \
        const QFileInfo &currentVideoFileInfo = videoSrcDirIterator.fileInfo(); \
        QStringList ffmpegCopyAndMuxArgs; \
        ffmpegCopyAndMuxArgs << "-f" << "h264" << "-r" << videoFps << "-i" << currentVideoFileInfo.absoluteFilePath() << "-an" << "-vcodec" << "copy"; \
        if(ffmpegTruncateArgOrZeroIfToNotSpecify != 0) \
        { \
            ffmpegCopyAndMuxArgs << "-t" << QString::number(ffmpegTruncateArgOrZeroIfToNotSpecify); \
        } \
        ffmpegCopyAndMuxArgs << (copyMuxToDestDir_WithSlashAppended + currentVideoFileInfo.completeBaseName() + ".mkv"); \
        QProcess process; \
        process.start("ffmpeg", ffmpegCopyAndMuxArgs, QIODevice::ReadOnly); \
        QString dupeWarningMb("WARNING: some files may have already been copied/muxed into " + QString(copyMuxToDestDir_WithSlashAppended) + " , and re-running this app again may create dupes in a different/new folder (ignore this warning if the dir just mentioned is a temp dir)"); \
        if(!process.waitForStarted(-1)) \
        { \
            DUMP_PROCESS_OUTPUT(process) \
            qDebug() << "ffmpeg failed to start with args" << ffmpegCopyAndMuxArgs; \
            qDebug() << dupeWarningMb; \
            return 1; \
        } \
        if(!process.waitForFinished(-1)) \
        { \
            DUMP_PROCESS_OUTPUT(process) \
            qDebug() << "ffmpeg failed to finish with args" << ffmpegCopyAndMuxArgs; \
            qDebug() << dupeWarningMb; \
            return 1; \
        } \
        DUMP_PROCESS_OUTPUT(process) \
        if(process.exitCode() != 0 || process.exitStatus() != QProcess::NormalExit) \
        { \
            qDebug() << "ffmpeg exitted abnormally with exit code:" << process.exitCode() << " -- args:" << ffmpegCopyAndMuxArgs; \
            qDebug() << dupeWarningMb; \
            return 1; \
        } \
    } \
}

#define RETURN_ONE_IF_TOUCHING_ALL_VIDEO_FILES_IN_DIR_USING_TIMESTAMPS_PARSED_FROM_FILENAMES_FAILS(dirOfVideosToTouch) \
{ \
    QDirIterator videoFilesToTouchIterator(dirOfVideosToTouch, (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden)); \
    while(videoFilesToTouchIterator.hasNext()) \
    { \
        videoFilesToTouchIterator.next(); \
        const QFileInfo &currentVideoFileInfo = videoFilesToTouchIterator.fileInfo(); \
        if(!currentVideoFileInfo.isFile()) /*should never happen*/ \
            continue; \
        QProcess process; \
        QStringList touchProcessArgs; \
        QStringList fileNameExcludingExtensionSplitAtDots = currentVideoFileInfo.completeBaseName().split("."); /*TODOreq: my go outside script needs to be updated to make files called "outsideAt.unixTimestamp.h264" -- it also needs time sanity checking*/ \
        if(fileNameExcludingExtensionSplitAtDots.size() < 1) \
        { \
            qDebug() << "filename didn't have enough parts to have a unix time" << currentVideoFileInfo.fileName(); \
            return 1; \
        } \
        bool convertOk = false; \
        qint64 unixTimestampSecondsMb = fileNameExcludingExtensionSplitAtDots.last().toLongLong(&convertOk); \
        if(!convertOk) \
        { \
            qDebug() << "second to last part (dots) was not a unix timestamp" << currentVideoFileInfo.fileName(); \
            return 1; \
        } \
        touchProcessArgs << QString("--date=@" + QString::number(unixTimestampSecondsMb)) << "--no-create" << videoFilesToTouchIterator.filePath(); \
        process.start("touch", touchProcessArgs, QIODevice::ReadOnly); \
        if(!process.waitForStarted(-1)) \
        { \
            DUMP_PROCESS_OUTPUT(process) \
            qDebug() << "touch failed to start:" << videoFilesToTouchIterator.filePath(); \
            return 1; \
        } \
        if(!process.waitForFinished(-1)) \
        { \
            DUMP_PROCESS_OUTPUT(process) \
            qDebug() << "touch failed to finish:" << videoFilesToTouchIterator.filePath(); \
            return 1; \
        } \
        DUMP_PROCESS_OUTPUT(process) \
        if(process.exitCode() != 0 || process.exitStatus() != QProcess::NormalExit) \
        { \
            qDebug() << "touch exitted abnormally with exit code:" << process.exitCode(); \
            return 1; \
        } \
    } \
}

#define RETURN_ONE_IF_GENERATING_AUDIO_DELAYS_FILE_FAILS(theAudioDelaysFile) \
{ \
    /*mux h264 on videoSource to mkv in tempDir with -t specified*/ \
    QTemporaryDir tempDirForTruncatedMuxedVideosForAudioDelaysFileCalculation; /*TO DOnemb: speed up deletion (as soon as we don't need it). TO DOnereq: parse/touch filenames*/ \
    if(!tempDirForTruncatedMuxedVideosForAudioDelaysFileCalculation.isValid()) \
    { \
        qDebug() << "failed to get temp dir for muxing the video for calculating the audio delays file:" << tempDirForTruncatedMuxedVideosForAudioDelaysFileCalculation.path(); \
        return 1; \
    } \
    QString tempPathForTruncatedMuxedVideosForAudioDelaysFileCalculation = appendSlashIfNeeded(tempDirForTruncatedMuxedVideosForAudioDelaysFileCalculation.path()); \
    RETURN_ONE_IF_ANY_FILE_IN_VIDEOSOURCE_FAILS_TO_COPYMUX(videoSource, tempPathForTruncatedMuxedVideosForAudioDelaysFileCalculation, 15) \
    RETURN_ONE_IF_TOUCHING_ALL_VIDEO_FILES_IN_DIR_USING_TIMESTAMPS_PARSED_FROM_FILENAMES_FAILS(tempPathForTruncatedMuxedVideosForAudioDelaysFileCalculation) \
    /*TODOoptimization: mux mp3 on audioSource to flac in tempDir with -t specified (keeps reads from audioSource to a minimum) -- maybe -t should be larger than normal to account for the looping/sync'ing process? at some -t, it's smarter to just do the full backup/copy lol*/ \
    QProcess muxerSyncerProcess; \
    muxerSyncerProcess.setInputChannelMode(QProcess::ForwardedInputChannel); /*woot 5.2 <3*/ \
    muxerSyncerProcess.setProcessChannelMode(QProcess::ForwardedChannels); \
    QStringList interactiveMuxerSyncerArgs; \
    if(!theAudioDelaysFile.open()) \
    { \
        qDebug() << "failed to create temporary audio delays file"; \
        return 1; \
    } \
    theAudioDelaysFile.close(); /*we only wanted it to be created (UNIQUELY), didn't want it to be opened*/ \
    interactiveMuxerSyncerArgs << audioSource << tempPathForTruncatedMuxedVideosForAudioDelaysFileCalculation << "--interactively-calculate-audio-delays-to-file" << theAudioDelaysFile.fileName(); \
    muxerSyncerProcess.start(muxerSyncerBinaryFilePath, interactiveMuxerSyncerArgs); \
    if(!muxerSyncerProcess.waitForStarted(-1)) \
    { \
        qDebug() << "interactive muxer syncer failed to start with these args:" << interactiveMuxerSyncerArgs; \
        return 1; \
    } \
    if(!muxerSyncerProcess.waitForFinished(-1)) \
    { \
        qDebug() << "interactive muxer syncer failed to start with these args:" << interactiveMuxerSyncerArgs; \
        return 1; \
    } \
    if(muxerSyncerProcess.exitCode() != 0 || muxerSyncerProcess.exitStatus() != QProcess::NormalExit) \
    { \
        qDebug() << "interactive muxer syncer exitted abnormally with exit code:" << muxerSyncerProcess.exitCode(); \
        return 1; \
    } \
}

#define RETURN_ONE_IF_RSYNC_SINGLE_FILE_FAILS(theSrcDir, theSrcFilePathRelativeToSrcDir, theDestDirWithSlashAppended_OrDestFilePath) \
if(!runProcess(QString(rsyncCmdPrefix " ./") + theSrcFilePathRelativeToSrcDir + QString(" ") + theDestDirWithSlashAppended_OrDestFilePath, theSrcDir)) \
{ \
    qDebug() << "failed to copy:" << theSrcFilePathRelativeToSrcDir << "to:" << theDestDirWithSlashAppended_OrDestFilePath; \
    return 1; \
}

#define RETURN_ONE_IF_PUTTING_AUDIO_DELAYS_FILE_NEXT_TO_A_V_FOLDERS_FAILS(theAudioDelaysFile, targetAudioDelaysFilePath) \
QFile audioDelaysFileEmptyChecker(theAudioDelaysFile.fileName()); \
if(!audioDelaysFileEmptyChecker.open(QIODevice::ReadOnly | QIODevice::Text)) \
{ \
    qDebug() << "failed to open audio delays file for checking if it's empty:" << theAudioDelaysFile.fileName(); \
    return 1; \
} \
bool audioDelaysFileIsEmpty = true; \
{ \
    QTextStream audioDelaysFileTextStream(&audioDelaysFileEmptyChecker); \
    while(!audioDelaysFileTextStream.atEnd()) \
    { \
        if(!audioDelaysFileTextStream.readLine().isEmpty()) \
        { \
            audioDelaysFileIsEmpty = false; \
            break; \
        } \
    } \
} \
audioDelaysFileEmptyChecker.close(); \
if(!audioDelaysFileIsEmpty) \
{ \
    QFileInfo theAudioDelayFileInfo(theAudioDelaysFile.fileName()); \
    RETURN_ONE_IF_RSYNC_SINGLE_FILE_FAILS(theAudioDelayFileInfo.absolutePath(), theAudioDelayFileInfo.fileName(), targetAudioDelaysFilePath) \
} \
else \
{ \
    qDebug() << "audio delays file was empty, so not moving:" << theAudioDelaysFile.fileName(); \
}

QString appendSlashIfNeeded(const QString &inputString)
{
    if(inputString.endsWith("/"))
    {
        return inputString;
    }
    return inputString + "/";
}
QString removeTrailingSlashIfNeeded(const QString &inputString)
{
    if(inputString.endsWith("/"))
    {
        return inputString.left(inputString.length()-1);
    }
    return inputString;
}

#define DESIRED_FILE_READ_ONLY_PERMISSIONS (QFileDevice::ReadOwner | QFileDevice::ReadGroup | QFileDevice::ReadOther | QFileDevice::ReadUser)
#define DESIRED_FILE_READWRITE_FOR_OWNERUSER_ONLY_PERMISSIONS (QFileDevice::ReadOwner | QFileDevice::ReadGroup | QFileDevice::ReadOther | QFileDevice::ReadUser | QFile::WriteOwner | QFile::WriteUser)
#define DESIRED_FOLDER_0755_PERMISSIONS (QFileDevice::ReadGroup | QFileDevice::ReadOwner | QFileDevice::ReadOther | QFileDevice::ReadUser | QFileDevice::WriteOwner | QFileDevice::WriteUser | QFileDevice::ExeOwner | QFileDevice::ExeOther | QFileDevice::ExeGroup | QFileDevice::ExeUser)
void setPermissionsToReadOnlyAndGiveWarningIfItFails(const QString &absolutePathToRecursivelySetAllFilesToReadOnlyIn, const QStringList &fileEntriesToSetToWriteOwnerUserAlso)
{
    int characterLengthOfAbsolutePathToRecursivelySetAllFilesToReadOnlyIn_WithSlashAppended = appendSlashIfNeeded(absolutePathToRecursivelySetAllFilesToReadOnlyIn).length();
    QDirIterator dirIterator(absolutePathToRecursivelySetAllFilesToReadOnlyIn, (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden), QDirIterator::Subdirectories);
    while(dirIterator.hasNext())
    {
        dirIterator.next();
        const QFileInfo &currentFileInfo = dirIterator.fileInfo();
        const QString &currentAbsoluteFilePath = currentFileInfo.absoluteFilePath();
        if(currentFileInfo.isFile())
        {
            const QString &relativeFilePath = currentAbsoluteFilePath.mid(characterLengthOfAbsolutePathToRecursivelySetAllFilesToReadOnlyIn_WithSlashAppended);
            if(!fileEntriesToSetToWriteOwnerUserAlso.contains(relativeFilePath))
            {
                //read-only
                if(currentFileInfo.permissions() != DESIRED_FILE_READ_ONLY_PERMISSIONS)
                {
                    if(!QFile::setPermissions(currentAbsoluteFilePath, DESIRED_FILE_READ_ONLY_PERMISSIONS))
                    {
                        qDebug() << "warning: could not set file to read-only:" << currentAbsoluteFilePath;
                        break; //chances are the rest won't work either
                    }
                }
            }
            else
            {
                //read-only for most, write for owner/user
                if(currentFileInfo.permissions() != DESIRED_FILE_READWRITE_FOR_OWNERUSER_ONLY_PERMISSIONS)
                {
                    if(!QFile::setPermissions(currentAbsoluteFilePath, DESIRED_FILE_READWRITE_FOR_OWNERUSER_ONLY_PERMISSIONS))
                    {
                        qDebug() << "warning: could not set file to read-only for most and readwrite for owner/user:" << currentAbsoluteFilePath;
                        break; //chances are the rest won't work either
                    }
                }
            }
            continue;
        }
        if(currentFileInfo.isDir())
        {
            //0755
            if(currentFileInfo.permissions() != DESIRED_FOLDER_0755_PERMISSIONS)
            {
                if(!QFile::setPermissions(currentAbsoluteFilePath, DESIRED_FOLDER_0755_PERMISSIONS))
                {
                    qDebug() << "warning: could not set folder to read-only:" << currentAbsoluteFilePath;
                    break; //chances are the rest won't work either
                }
            }
            continue;
        }
    }
}

//OT: the style I'm using in this is schmexy, having scoped/shared pointers do tasks when they go out of scope. The same kind of thing can be used in any other designs in a slot that is invoked in an asynchronous design. I tend to do sanity tests, and if any of them fail I do "emit doneDoingThisAsyncTask(false); return;" -- false to indicate failure ofc. I could use the scoped/shared pointers like I am in this app, where the destructor emits doneDoingThisAsyncTask(false), unless I set some bool (like CmdRunAtConstructionSucceeded used in this app) to true, in which case the "next async step/signal/slot/whatever" is emitted instead! For the final async slot, the only difference would be the "true" or "false" value of "success", but for the all the intermediate steps it could be "emit doneWith<x>(false)" vs. "[emit] proceedToStep<y>()". It would simply save lines of code, as in each sanity check I can just type "return;" instead of emitting the signal. Even though it would save lines of code, a con is that it decreases readability. Maybe would be useful in d=i, where it doesn't decrease understandability (although it still does in the generated C++ (but eh I am beginning to think that making the generated C++ might be wasted effort (idk I flip flop on the issue. ex: C++ started off as a C code generator... then ditched it later because it was too limiting))).

//TODOoptional: when to recursively gpg verify? here's an idea: a serialized list of all entries and when they were last gpg verified (note: the list itself is signed and verified after/before every use of it). at the end of this "import+mux" script, we (NOPE: look at the time. if it's before I'd be waking up by a few hours) recursively gpg verify maybe as many of those entries as possible for about ~30 minutes (obviously don't SIGKILL the gpg verify at 30 minutes... it can go over that) -- stop of course if all of them were verified in that time (will happen with small dirs). the "source" (either main backup dir or one of the replicas) used for the gpg verify is selected at random (different random selection for every entry). Using ~30 minutes after a "sync+mux at night" script is smart because then I never have to sit and wait for the gpg verifying to finish, I'll be asleep. Failed verifications need to be presented somewhere (maybe at the start of the next night's sync? or some morning time notification (beginning of morning script?)). This idea should probably be an entirely different app, and the command chain in bash is this: "BackupReplicateDeleteMuxAndUmountCli && RecursivelyGpgVerifyForAbout30MinsAndKeepRecordOfShit && xfce4-session-logout --fast --halt" (yes, i want to NOT shut down when when _anything_ fails (maybe i'll play a sound using more bash shit))

//This import/backup script is (had:becoming) a pain in my ass, I need/want to go outside!!! But then I remember that my going [with the hat on] outside is pointless if I can't come back home and import it!!! Hah!


/*
for lack of a better place (lol organization problems within organization problems (and a joke within (and an explaination next to it (and another 8..8)):

text/ (git repo, is 'textBare' on replicas)
binary/ (managed via this app and others like it)

the reason i want binaryMeta to not be in binary is so that i can cd into binary and do "rsync --etc --backup --prefix=~accidentallyOverwitten ./ /someCopy". if i were to keep the sigs in binary, then it would get overwritten every time and there would be tons of "accidentallyOverwritten" copies of the sigsfile. i could exclude it from the rsync and send it on it's own command but i think i also want versioning of it (i just don't want a bunch of "accidentallyOverwritten" files to be that versioning (sloppy))

this structure is also forwards-compatible with git auto submodules helper, once i get around to codan it (i doubt i'll use it with binaries)

tempted to have a 3rd [non-hidden] folder under files/ called "versionedBinaries". gimp xcf files, for example, would go in there. those are binaries that i DO want to use under git (worth the wait/overhead/etc).. but i still don't want it to slow down my text folder

TODOopimization: in binaryMeta could be a skeleton hierarchy copy of binary, but in each dir is just a single sigsfile. would just less then amount of re-writing of sigs that haven't changed i'll be doing in this first implementation. and i also need to add some logic to recursive gpg signer "don't rewrite the sigsfile if nothing has been added" (actually that's easy af so i can/should add it now)
//TODOoptional: intentionally over-use 'append/remove slashes if necessary'... because the user-input'd config may not have slashes in the right place
//TODOmb: keep N days in the past on audio/video source? knowing what is what becomes a [solvable] issues (could move them into dirs accordingly). it's just in case something goes wrong further down the line... it hopefully would be noticed before those N days. gotta make sure i don't run out of room ofc

maybe the determiner for whether or not text and binaryMeta have the word "Bare" appended is determined by whether or not the drive is a replica


NOPE:
files/.meta/binaryMeta (git repo for sigs, is 'binaryMetaBare' on replicas)
files/.meta/text (maybe this dir doesn't exist, but i was thinking of putting .lastModifiedTimestamps here (since it's so small, might as well keep it in files/text/ -- however if i wanted sigs of /text/ (no point since git, just sign commits/tags nvm nvm)))

*/

int main(int argc, char *argv[]) //TODOoptional: "minNumReplicas" cli arg (hardcoded list of replicas (or hell, provided via args even). we TRY to replicate to all of them (maybe a hardcoded max + randomization of which we select), but error out before even starting if at least minNumReplicas can't be mounted/replicated-to)
{
    QCoreApplication a(argc, argv);

    QFileInfo muxerSyncerBinaryFileInfo(muxerSyncerBinaryFilePath);
    if((!muxerSyncerBinaryFileInfo.exists()) || (!muxerSyncerBinaryFileInfo.isExecutable()))
    {
        //TODOmb: qbs has build-on-demand + run functionality
        qDebug() << "muxer syncer binary either doesn't exist or isn't executable:" << muxerSyncerBinaryFilePath;
        return 1;
    }
    QFileInfo recursiveGpgSignerBinaryFileInfo(recursiveGpgSignerBinaryFilePath);
    if((!recursiveGpgSignerBinaryFileInfo.exists()) || (!recursiveGpgSignerBinaryFileInfo.isExecutable()))
    {
        qDebug() << "recursive gpg signer either doesn't exist or isn't executable:" << recursiveGpgSignerBinaryFilePath;
        return 1;
    }

    //mount all -- TODOreq: ntfs drives need sudo xD
    RETURN_ONE_IF_CMD_RUN_AT_CONSTRUCTOR_OF_SMART_PROCESS_THINGO_FAILS(audioSourceMount, mountCmdPrefix audioSourceMountPoint, umountCmdPrefix audioSourceMountPoint)
    RETURN_ONE_IF_CMD_RUN_AT_CONSTRUCTOR_OF_SMART_PROCESS_THINGO_FAILS(videoSourceMount, mountCmdPrefix videoSourceMountPoint, umountCmdPrefix videoSourceMountPoint)
    RETURN_ONE_IF_CMD_RUN_AT_CONSTRUCTOR_OF_SMART_PROCESS_THINGO_FAILS(backupMount, sudoMountCmdPrefix backupMountPoint, sudoUmountCmdPrefix backupMountPoint)
    //do the same, but for the replicas
    allReplicas
    QListIterator<QPair<QString, QString> > replicasIterator(replicas);
    QList<QSharedPointer<BeforeAndAfterProcessRunner> > replicaMounts;
    int numReplicasMounted = 0;
    while(replicasIterator.hasNext())
    {
        const QPair<QString,QString> &currentReplica = replicasIterator.next();
        //atm all my replicas are ntfs, so they require sudo... but eh i shouldn't issue it if it's not needed (and I'm considering switching every drive I have to ext4 because fuck that other OS :-P)
        RETURN_ONE_IF_CMD_RUN_AT_CONSTRUCTOR_OF_SMART_PROCESS_THINGO_FAILS_LIST_VARIANT(replicaMounts, mountCmdPrefix + currentReplica.first, umountCmdPrefix + currentReplica.first)
        ++numReplicasMounted;
    }
    if(numReplicasMounted < 1)
    {
        qDebug() << "there weren't at least 1 replicas available"; //TODOoptional: use minReplicas intelligently. entries in the list of replicas should be allowed to not be availalbe (so long as minReplicas is met), whereas right now we error out on the first unavailable replica. also, shuffle() the replicas for load balancing :-P
        return 1;
    }
    QString airGapTempDestRoot = replicas.last().first + airGapReplicaTempDirForMuxedCopiesRelativeToMountPoint;
    QFileInfo airGapReplicaTempDirFileInfo(airGapTempDestRoot);
    if((!airGapReplicaTempDirFileInfo.exists()) || ((!airGapReplicaTempDirFileInfo.isDir())) || (!airGapReplicaTempDirFileInfo.isWritable()))
    {
        qDebug() << "air gap replica temp dir either doesn't exist or isn't a dir or isn't writeable:" << airGapTempDestRoot;
        return 1;
    }


    //prepare to backup, ensure sources and dest exist and that we can write to them
    RETURN_ONE_IF_DIR_IS_NOT_GOOD(videoSource)
    RETURN_ONE_IF_DIR_IS_NOT_GOOD(audioSource)
    RETURN_ONE_IF_VIDEOSOURCE_DIR_HAS_ANY_NONFILES_OR_NONH264FILES(videoSource) //handling sub-directories is a pain, so not going to allow it for now
    RETURN_ONE_IF_DIR_IS_NOT_GOOD(backupDestOutsideBase)
    //do the same, but for the replicas
    replicasIterator.toFront();
    while(replicasIterator.hasNext())
    {
        const QPair<QString,QString> &currentReplica = replicasIterator.next();
        RETURN_ONE_IF_DIR_IS_NOT_GOOD(currentReplica.first)
        RETURN_ONE_IF_DIR_IS_NOT_GOOD(currentReplica.second)
    }


    //calculate/generate audio delays file
    QTemporaryFile audioDelaysFile;
    //run the muxer syncer in interactive mode to get audio delay timestamps (we use the audio delays file later, and also save it next to the 'a' and 'v' folders (TODOblah: such a file should surely be in revision control fml since we're talking-about/handling /binary/)). I would rather call it as a lib, but mixing sync+async is easiest to do with a QProcess (another thread + wait condition works too (and keeps me in 'lib' mode), but is surely not 'easier')
    RETURN_ONE_IF_GENERATING_AUDIO_DELAYS_FILE_FAILS(audioDelaysFile) //TODOmb: if replicas.isEmpty(), perhaps we should skip this step... since ultimately it means we won't be mux'ing to a replica for the morning script (no muxing = no need to av sync)


    //prepare to backup, make the dest dirs, bailing out if any one of them fails to be made (or is already made)
    QString unixTimeInSecondsNow = QString::number(QDateTime::currentMSecsSinceEpoch()/1000);
    QString backupDestNewSubDir_WithSlashAppended = backupDestPrefix + unixTimeInSecondsNow + QDir::separator();
    QString backupDest_WithSlashAppended = backupDestOutsideBase + backupDestNewSubDir_WithSlashAppended;
    QString backupDestAudioDir_WithSlashAppended(backupDest_WithSlashAppended + backupDestAudioSubDir + QDir::separator());
    QString backupDestVideoDir_WithSlashAppended(backupDest_WithSlashAppended + backupDestVideoSubDir + QDir::separator());
    //QDir backupDestDir(backupDest_WithSlashAppended);
    RETURN_ONE_IF_MKDIR_AT_CONSTRUCTOR_OF_SMART_MKDIR_THINGO_FAILS(newlyMadeBackupDestDir, backupDest_WithSlashAppended)
    RETURN_ONE_IF_MKDIR_AT_CONSTRUCTOR_OF_SMART_MKDIR_THINGO_FAILS(newlyMadeBackupDestAudioDir, backupDestAudioDir_WithSlashAppended)
    RETURN_ONE_IF_MKDIR_AT_CONSTRUCTOR_OF_SMART_MKDIR_THINGO_FAILS(newlyMadeBackupDestVideoDir, backupDestVideoDir_WithSlashAppended)
    //do the same, but for the replicas
    replicasIterator.toFront();
    QList<QSharedPointer<AutoRemovingDirMker> > replicaDirs;
    while(replicasIterator.hasNext())
    {
        QString replicaBackupDest_WithSlashAppended = removeTrailingSlashIfNeeded(replicasIterator.next().first) + outsidePathRelativeToMountPoint + backupDestNewSubDir_WithSlashAppended;
        RETURN_ONE_IF_MKDIR_AT_CONSTRUCTOR_OF_SMART_MKDIR_THINGO_FAILS_LIST_VARIANT(replicaDirs, replicaBackupDest_WithSlashAppended)
    }
    //do the same, but for the air gap replica
    QString airGapTempDestPath = airGapTempDestRoot + backupDestNewSubDir_WithSlashAppended;
    RETURN_ONE_IF_MKDIR_AT_CONSTRUCTOR_OF_SMART_MKDIR_THINGO_FAILS(newlyMadeAirGapTempDestDir, airGapTempDestPath)


    //since every mkdir succeeded, set "autoRemove" on the dirs to false (it would fail if there's content in it anyways, but still) -- TODOmb: should this go later, after the backup (and/or replicate (respectively))? if a copy fails mid-list-of-files, having it be auto-remove would fail since it's just rmdir'ing, idfk how i want it to fail
    newlyMadeBackupDestDir->AutoRemove = false;
    newlyMadeBackupDestAudioDir->AutoRemove = false;
    newlyMadeBackupDestVideoDir->AutoRemove = false;
    //do the same, but for replicas
    QListIterator<QSharedPointer<AutoRemovingDirMker> > replicaDirsIterator(replicaDirs);
    while(replicaDirsIterator.hasNext())
    {
        replicaDirsIterator.next()->AutoRemove = false;
    }
    newlyMadeAirGapTempDestDir->AutoRemove = false;


    //============DO THE ACTUAL BACKUP=========
    RETURN_ONE_IF_RSYNC_COPY_CMD_FAILS(audioSource, backupDestAudioDir_WithSlashAppended)
    //RETURN_ONE_IF_RSYNC_COPY_CMD_FAILS(videoSource, backupDestVideoDir_WithSlashAppended)
    RETURN_ONE_IF_ANY_FILE_IN_VIDEOSOURCE_FAILS_TO_COPYMUX(videoSource, backupDestVideoDir_WithSlashAppended, 0) //because ffprobe can't do raw h264, i mux and copy the video files (into mkv) at the same time. doing this allows me to change the hardcoded fps in the future without losing the fps for the vids made in the past
    RETURN_ONE_IF_TOUCHING_ALL_VIDEO_FILES_IN_DIR_USING_TIMESTAMPS_PARSED_FROM_FILENAMES_FAILS(backupDestVideoDir_WithSlashAppended)

    //backup the audio delays file next to the a and v folders
    QString audioDelaysFileName = backupDest_WithSlashAppended + "audioDelays.ini";
    RETURN_ONE_IF_PUTTING_AUDIO_DELAYS_FILE_NEXT_TO_A_V_FOLDERS_FAILS(audioDelaysFile, audioDelaysFileName)
    /*first check it isn't empty*/


    //try to set to read-only, buys just a tiny bit of overwrite protection
    setPermissionsToReadOnlyAndGiveWarningIfItFails(backupMountPoint destPathBinaryRoot, QStringList() << allSigsFileName);


    //run recursive gpg signer. it will detect most overwrites (we don't want to replicate overwrites), woot. recursive gpg signer serves doubly as a cheap af way to verify last modified timestamps for a hierarchy... but with the added bonus that it signs new files it sees :-D. the best protection would be to run recursive sign + verify here, so we KNOW there weren't any overwrites.... however that is slow as fuuuuuuuck to do every time....
    QProcess recursiveGpgSignerProcess;
    recursiveGpgSignerProcess.setProcessChannelMode(QProcess::ForwardedChannels);
    QStringList recursiveGpgSignerArgs;
    recursiveGpgSignerArgs << backupMountPoint destPathBinaryRoot << backupMountPoint sigsFilePathRelativeToMountPoint;
    recursiveGpgSignerProcess.start(recursiveGpgSignerBinaryFilePath, recursiveGpgSignerArgs, QIODevice::ReadOnly);
    if(!recursiveGpgSignerProcess.waitForStarted(-1))
    {
        qDebug() << "recursive gpg signer failed to start with args:" << recursiveGpgSignerArgs;
        return 1;
    }
    if(!recursiveGpgSignerProcess.waitForFinished(-1))
    {
        qDebug() << "recursive gpg signer failed to finish with args:" << recursiveGpgSignerArgs;
        return 1;
    }
    if((recursiveGpgSignerProcess.exitCode() != 0) || (recursiveGpgSignerProcess.exitStatus() != QProcess::NormalExit))
    {
        qDebug() << "recursive gpg signer exitted abnormally with exit code:" << recursiveGpgSignerProcess.exitCode();
        return 1;
    }


    //replicate
    replicasIterator.toFront();
    QString hackyLoadBalancerChangingSourceHehForEachReplicaHeh = backupMountPoint destPathBinaryRoot; //at first backup dest is used as src, but then we use the first replica as the source, and so on. TODOoptional: a flag for a replica "don't use as source in load balancing" (usb [2.0] drives) -- atm I only have one replica usb drive and it's the very last replica anyways lolol
    while(replicasIterator.hasNext())
    {
        const QPair<QString,QString> &currentReplica = replicasIterator.next();
        QString replicaDestWithSlashAppended = appendSlashIfNeeded(currentReplica.second);
        RETURN_ONE_IF_RSYNC_COPY_CMD_FAILS(hackyLoadBalancerChangingSourceHehForEachReplicaHeh, replicaDestWithSlashAppended)
        //replicate the sigs file (it was excluded from the regular replicate command because we DON'T want to backup-on-overwrite it
        RETURN_ONE_IF_RSYNC_SINGLE_FILE_FAILS(hackyLoadBalancerChangingSourceHehForEachReplicaHeh, allSigsFileName, replicaDestWithSlashAppended)
        setPermissionsToReadOnlyAndGiveWarningIfItFails(replicaDestWithSlashAppended, QStringList() << allSigsFileName);
        hackyLoadBalancerChangingSourceHehForEachReplicaHeh = replicaDestWithSlashAppended;
    }


    //TODOoptional: I suppose this is where the "gpg verify randomly (but in a fashion that ensures each file is verified before looping around (and additionally, each file is 'randomly' verified on EACH replica before the same replica is checked again)) for ~30 mins" would go... because we do need the replicas to be mounted -- any file on any replica doesn't gpg verify = return 1 (don't shutdown. i want to see that shit in the morning asep)


    //unmount most replicas
    QSharedPointer<BeforeAndAfterProcessRunner> airGapDriveForTheMuxedCopiesForMorningScript;
    if(!replicaMounts.isEmpty())
    {
        airGapDriveForTheMuxedCopiesForMorningScript = replicaMounts.last(); //ref
    }
    replicaMounts.clear(); //unmounts all replicas, except the airGap one ref'd above :)


    //delete sources (ffffffff one typo and I'm dead (is why I exit on ANY error))
    RETURN_ONE_IF_RECURSIVE_DELETE_OR_REMAKE_FAILS(audioSource)
    RETURN_ONE_IF_RECURSIVE_DELETE_OR_REMAKE_FAILS(videoSource)


    //unmount sources
    audioSourceMount.reset();
    videoSourceMount.reset();


    //mux
    //oh boy, the hardest part (by far) -- might as well go into OO mode since the muxing part might be re-used and I'd been planning on doing it in Qt anyways...
#if 0
    if(airGapDriveForTheMuxedCopiesForMorningScript.isNull())
    {
        //will never happen
        qDebug() << "no replica available for muxing to. done copying to backup drive only";
        //return 0; -- third time i've flip-flopped on this. we WANT replicas! TO DOnereq: fail before even starting backup if no replicas in the replica list?
        return 1;
    }
#endif
    //mux directly to the usb in temp/dist dir for Morning script (TODOreq: delete them at the end of Morning script)
    QProcess muxSyncProcess;
    muxSyncProcess.setProcessChannelMode(QProcess::ForwardedChannels); //OT'ish: lol i'm dumb for doing this manually (but when i pass them to signals o/e, then i have to do it manually)
    QStringList muxSyncArgs;
    muxSyncArgs << backupDestAudioDir_WithSlashAppended << backupDestVideoDir_WithSlashAppended << airGapTempDestPath << "--use-audio-delays-from-file" << audioDelaysFileName;
#define MUX_SYNCER_ADD_FFMPEG_ARG "--add-ffmpeg-arg"
    muxSyncArgs << MUX_SYNCER_ADD_FFMPEG_ARG << "-acodec" << MUX_SYNCER_ADD_FFMPEG_ARG << "opus" << MUX_SYNCER_ADD_FFMPEG_ARG << "-b:a" << MUX_SYNCER_ADD_FFMPEG_ARG << "32k" << MUX_SYNCER_ADD_FFMPEG_ARG << "-ac" <<  MUX_SYNCER_ADD_FFMPEG_ARG << "1" << MUX_SYNCER_ADD_FFMPEG_ARG << "-s" << MUX_SYNCER_ADD_FFMPEG_ARG << "720x480" << MUX_SYNCER_ADD_FFMPEG_ARG << "-b:v" << MUX_SYNCER_ADD_FFMPEG_ARG << "275k" << MUX_SYNCER_ADD_FFMPEG_ARG << "-vcodec" << MUX_SYNCER_ADD_FFMPEG_ARG << "theora" << MUX_SYNCER_ADD_FFMPEG_ARG << "-r" << MUX_SYNCER_ADD_FFMPEG_ARG << "10" << MUX_SYNCER_ADD_FFMPEG_ARG << "-f" << MUX_SYNCER_ADD_FFMPEG_ARG << "segment" << MUX_SYNCER_ADD_FFMPEG_ARG << "-segment_time" << MUX_SYNCER_ADD_FFMPEG_ARG << "180" << MUX_SYNCER_ADD_FFMPEG_ARG << "-segment_list_size" << MUX_SYNCER_ADD_FFMPEG_ARG << "999999999" << MUX_SYNCER_ADD_FFMPEG_ARG << "-segment_wrap" << MUX_SYNCER_ADD_FFMPEG_ARG << "999999999" << MUX_SYNCER_ADD_FFMPEG_ARG << "-segment_list" << MUX_SYNCER_ADD_FFMPEG_ARG << QString(airGapTempDestPath + "/%VIDEOBASENAME%-segmentEntryList.txt") << MUX_SYNCER_ADD_FFMPEG_ARG << "-reset_timestamps" << MUX_SYNCER_ADD_FFMPEG_ARG << "1" << "--mux-to-ext" << "-%d.ogg";
    //TODOreq: lutyuv brightness? being outside maybe not necessary (idfk) -- also: noir for night time shenigans (a realtime preview would come in handy too)!
    muxSyncProcess.start(muxerSyncerBinaryFilePath, muxSyncArgs);
    if(!muxSyncProcess.waitForStarted(-1))
    {
        qDebug() << "mux syncer failed to start with args:" << muxSyncArgs;
        return 1;
    }
    if(!muxSyncProcess.waitForFinished(-1))
    {
        qDebug() << "mux syncer failed to finish with args:" << muxSyncArgs;
        return 1;
    }
    if(muxSyncProcess.exitCode() != 0 || muxSyncProcess.exitStatus() != QProcess::NormalExit)
    {
        qDebug() << "mux syncer exitted abnormally with exit code:" << muxSyncProcess.exitCode() << " with args:" << muxSyncArgs;
        return 1;
    }


    //unmount
    //the air gap drive gets unmounted when the shared pointer goes out of scope


    qDebug() << "backup replicate delete mux and unmount finished successfully -- everything OK";
    return 0;
}
