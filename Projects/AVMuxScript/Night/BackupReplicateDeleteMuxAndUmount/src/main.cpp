#include <QCoreApplication>

#include <QProcess>
#include <QDebug>
#include <QScopedPointer>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QDirIterator>
#include <QSharedPointer>


//MANDATORY config:
#define videoFps "10" /* this must be kept in sync with whatever the capture device is using and also the ffmpeg mux command in DirectoriesOfAudioAndVideoFilesMuxerSyncer. Don't change this value mid-day. only change it in between days (ie, at morning before recording, or at night _after_ running this script) */
#define videoSourceMountPoint "/mnt/videoSource"
#define videoSource videoSourceMountPoint "/goOutsideVids"

#define audioSourceMountPoint "/mnt/audioSource"
#define audioSource audioSourceMountPoint "/VOICE/FOLDER01"

#define destPathOnBackupAndReplicaMountPoints "/binary/outside"

#define backupMountPoint "/mnt/sdb"
#define backupDestBase backupMountPoint destPathOnBackupAndReplicaMountPoints

#define allReplicas \
replicas << qMakePair(QString("/mnt/sdc"), QString("/mnt/sdc") + destPathOnBackupAndReplicaMountPoints); \
replicas << qMakePair(QString("/mnt/blackCavalry"), QString("/mnt/blackCavalry") + destPathOnBackupAndReplicaMountPoints);

//optional config:
#define backupDestPrefix "/daySyncedAt-"
#define backupDestNewFolderForThisSync backupDestBase backupDestPrefix
#define backupDestAudioSubDir "a"
#define backupDestVideoSubDir "v"

#if 0 //corresponding fstab parts (for reference)
/dev/sdb1 /mnt/sdb auto rw,user,noauto 0 0
/dev/sdc1 /mnt/sdc auto rw,user,noauto 0 0
UUID=5d147465-f356-4d87-90cd-260b03305147 /mnt/videoSource ext4 rw,user,noauto 0 0
LABEL=IC\040RECORDER /mnt/audioSource vfat rw,user,noauto 0 0
UUID=0A5C1C5C6C0566FD /mnt/blackCavalry auto rw,user,noauto 0 0
#endif


#define DUMP_PROCESS_OUTPUT \
qDebug() << process.readAllStandardError(); \
qDebug() << process.readAllStandardOutput();
static bool runProcess(const QString &cmd, const QString &workingDirectory_OrEmptyStringIfToNotSet = QString())
{
    QProcess process;
    if(!workingDirectory_OrEmptyStringIfToNotSet.isEmpty())
        process.setWorkingDirectory(workingDirectory_OrEmptyStringIfToNotSet);
    //process.setProcessChannelMode(QProcess::MergedChannels);
    qDebug() << "running command:" << cmd;
    process.start(cmd);
    if(!process.waitForStarted(-1))
    {
        DUMP_PROCESS_OUTPUT
        qDebug() << "failed to start:" << cmd;
        return false;
    }
    if(!process.waitForFinished(-1))
    {
        DUMP_PROCESS_OUTPUT
        qDebug() << "failed to finish:" << cmd;
        return false;
    }
    DUMP_PROCESS_OUTPUT
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
#define rsyncCmdPrefix "rsync -avhh --progress --backup --suffix=~accidentallyOverwrittenDuringSyncAt-"
#define rsyncCmdMiddle " ./ "

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
if(!runProcess(rsyncCmdPrefix + unixTimeInSecondsNow + rsyncCmdMiddle + rsyncCmdDest, rsyncCmdSource)) \
{ \
    qDebug() << "failed to backup (" << rsyncCmdSource << ") to (" << rsyncCmdDest << ")"; \
    return 1; \
}

#define GIVE_WARNING_IF_SETTINGS_PERMISSIONS_TO_READ_ONLY_FAILS(dirToRecursivelySetAllFilesToReadOnlyIn) \
{ \
    QDirIterator dirIterator(dirToRecursivelySetAllFilesToReadOnlyIn, (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden), QDirIterator::Subdirectories); \
    while(dirIterator.hasNext()) \
    { \
        dirIterator.next(); \
        const QFileInfo &currentFileInfo = dirIterator.fileInfo(); \
        if(!currentFileInfo.isFile()) \
            continue; \
        if(!QFile::setPermissions(currentFileInfo.absoluteFilePath(), (QFileDevice::ReadOwner | QFileDevice::ReadGroup | QFileDevice::ReadOther))) \
        { \
            qDebug() << "warning, could not set file to read-only:" << currentFileInfo.filePath(); \
            break; /*chances are the rest won't work either*/ \
        } \
    } \
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

#define RETURN_ONE_IF_ANY_FILE_IN_VIDEOSOURCE_FAILS_TO_COPYMUX(theVideoSourceDir) \
{ \
    QDirIterator videoSrcDirIterator(theVideoSourceDir, (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden)/*, QDirIterator::Subdirectories*/); \
    while(videoSrcDirIterator.hasNext()) \
    { \
        videoSrcDirIterator.next(); \
        const QFileInfo &currentVideoFileInfo = videoSrcDirIterator.fileInfo(); \
        QStringList ffmpegCopyAndMuxArgs; \
        ffmpegCopyAndMuxArgs << "-f" << "h264" << "-r" << videoFps << "-i" << currentVideoFileInfo.absoluteFilePath() << "-an" << "-vcodec" << "copy" << (backupDestVideoDir_WithSlashAppended + currentVideoFileInfo.completeBaseName() + ".mkv"); \
        QProcess process; \
        process.start("ffmpeg", ffmpegCopyAndMuxArgs, QIODevice::ReadOnly); \
        if(!process.waitForStarted(-1)) \
        { \
            DUMP_PROCESS_OUTPUT \
            qDebug() << "ffmpeg failed to start with args" << ffmpegCopyAndMuxArgs; \
            qDebug() << "some files may have already been copied/muxed into backup dest, and re-running this app may create dupes"; \
            return 1; \
        } \
        if(!process.waitForFinished(-1)) \
        { \
            DUMP_PROCESS_OUTPUT \
            qDebug() << "ffmpeg failed to finish with args" << ffmpegCopyAndMuxArgs; \
            qDebug() << "some files may have already been copied/muxed into backup dest, and re-running this app may create dupes"; \
            return 1; \
        } \
        DUMP_PROCESS_OUTPUT \
        if(process.exitCode() != 0 || process.exitStatus() != QProcess::NormalExit) \
        { \
            qDebug() << "ffmpeg exitted abnormally with exit code:" << process.exitCode() << " -- args:" << ffmpegCopyAndMuxArgs; \
            qDebug() << "some files may have already been copied/muxed into backup dest, and re-running this app may create dupes"; \
            return 1; \
        } \
    } \
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

//OT: the style I'm using in this is schmexy, having scoped/shared pointers do tasks when they go out of scope. The same kind of thing can be used in any other designs in a slot that is invoked in an asynchronous design. I tend to do sanity tests, and if any of them fail I do "emit doneDoingThisAsyncTask(false); return;" -- false to indicate failure ofc. I could use the scoped/shared pointers like I am in this app, where the destructor emits doneDoingThisAsyncTask(false), unless I set some bool (like CmdRunAtConstructionSucceeded used in this app) to true, in which case the "next async step/signal/slot/whatever" is emitted instead! For the final async slot, the only difference would be the "true" or "false" value of "success", but for the all the intermediate steps it could be "emit doneWith<x>(false)" vs. "[emit] proceedToStep<y>()". It would simply save lines of code, as in each sanity check I can just type "return;" instead of emitting the signal. Even though it would save lines of code, a con is that it decreases readability. Maybe would be useful in d=i, where it doesn't decrease understandability (although it still does in the generated C++ (but eh I am beginning to think that making the generated C++ might be wasted effort (idk I flip flop on the issue. ex: C++ started off as a C code generator... then ditched it later because it was too limiting))).

//TODOreq: when go recursively gpg verify? here's an idea: a serialized list of all entries and when they were last gpg verified (note: the list itself is signed and verified after/before every use of it). at the end of this "import+mux" script, we (NOPE: look at the time. if it's before I'd be waking up by a few hours) recursively gpg verify maybe as many of those entries as possible for about ~30 minutes (obviously don't SIGKILL the gpg verify at 30 minutes... it can go over that) -- stop of course if all of them were verified in that time (will happen with small dirs). the "source" (either main backup dir or one of the replicas) used for the gpg verify is selected at random (different random selection for every entry). Using ~30 minutes after a "sync+mux at night" script is smart because then I never have to sit and wait for the gpg verifying to finish, I'll be asleep. Failed verifications need to be presented somewhere (maybe at the start of the next night's sync? or some morning time notification (beginning of morning script?)). This idea should probably be an entirely different app, and the command chain in bash is this: "BackupReplicateDeleteMuxAndUmountCli && RecursivelyGpgVerifyForAbout30MinsAndKeepRecordOfShit && xfce4-session-logout --fast --halt" (yes, i want to NOT shut down when when _anything_ fails (maybe i'll play a sound using more bash shit))

//This import/backup script is (had:becoming) a pain in my ass, I need/want to go outside!!! But then I remember that my going [with the hat on] outside is pointless if I can't come back home and import it!!! Hah!


/*
for lack of a better place (lol organization problems within organization problems (and a joke within (and an explaination next to it (and another 8..8)):

files/
files/text/ (git repo, can be 'textBare' on some drives)
files/binary/
files/.meta/binaryMeta (git repo for sigs, can be 'binaryMetaBare' on some drives)

the reason i want binaryMeta to not be in binary is so that i can cd into binary and do "rsync --etc --backup --prefix=~accidentallyOverwitten ./ /someCopy". if i were to keep the sigs in binary, then it would get overwritten every time and there would be tons of "accidentallyOverwritten" copies of the sigsfile. i could exclude it from the rsync and send it on it's own command but i think i also want versioning of it (i just don't want a bunch of "accidentallyOverwritten" files to be that versioning (sloppy))

this structure is also forwards-compatible with git auto submodules helper, once i get around to codan it (i doubt i'll use it with binaries)

tempted to have a 3rd [non-hidden] folder under files/ called "versionedBinaries". gimp xcf files, for example, would go in there. those are binaries that i DO want to use under git (worth the wait/overhead/etc).. but i still don't want it to slow down my text folder

TODOopimization: in binaryMeta could be a skeleton hierarchy copy of binary, but in each dir is just a single sigsfile. would just less then amount of re-writing of sigs that haven't changed i'll be doing in this first implementation. and i also need to add some logic to recursive gpg signer "don't rewrite the sigsfile if nothing has been added" (actually that's easy af so i can/should add it now)

maybe the determiner for whether or not text and binaryMeta have the word "Bare" appended is determined by whether or not the drive is a replica


NOPE:
files/.meta/text (maybe this dir doesn't exist, but i was thinking of putting .lastModifiedTimestamps here (since it's so small, might as well keep it in files/text/ -- however if i wanted sigs of /text/ (no point since git, just sign commits/tags nvm nvm)))

*/

int main(int argc, char *argv[]) //TODOoptional: "minNumReplicas" cli arg (hardcoded list of replicas (or hell, provided via args even). we TRY to replicate to all of them (maybe a hardcoded max + randomization of which we select), but error out before even starting if at least minNumReplicas can't be mounted/replicated-to)
{
    //TODOmb: a QTemporaryDir guarantees me to not overwrite another dir, but I wish I could ...... OH WAIT mkdir (not path) fails if dir exists! woot! So I should make the dirs on the backup and all replicas before beginning the backup. They all must succeed. What if just one fails, do I then rmdir the ones I created before that fail? (more fancy scoped destructor shits?). rmdir is actually not dangerous, because it fails if the dir isn't empty.

    QCoreApplication a(argc, argv);
    return 1;

    //mount all -- TODOreq: ntfs drives need sudo xD
    RETURN_ONE_IF_CMD_RUN_AT_CONSTRUCTOR_OF_SMART_PROCESS_THINGO_FAILS(audioSourceMount, mountCmdPrefix audioSourceMountPoint, umountCmdPrefix audioSourceMountPoint)
    RETURN_ONE_IF_CMD_RUN_AT_CONSTRUCTOR_OF_SMART_PROCESS_THINGO_FAILS(videoSourceMount, mountCmdPrefix videoSourceMountPoint, umountCmdPrefix videoSourceMountPoint)
    RETURN_ONE_IF_CMD_RUN_AT_CONSTRUCTOR_OF_SMART_PROCESS_THINGO_FAILS(backupMount, sudoMountCmdPrefix backupMountPoint, sudoUmountCmdPrefix backupMountPoint)
    //do the same, but for the replicas
    QList<QPair<QString /* replica mount point */, QString /* replica base dir */> > replicas;
    allReplicas
    QListIterator<QPair<QString, QString> > replicasIterator(replicas);
    QList<QSharedPointer<BeforeAndAfterProcessRunner> > replicaMounts;
    while(replicasIterator.hasNext())
    {
        const QPair<QString,QString> &currentReplica = replicasIterator.next();
        //atm all my replicas are ntfs, so they require sudo... but eh i shouldn't issue it if it's not needed (and I'm considering switching every drive I have to ext4 because fuck that other OS :-P)
        RETURN_ONE_IF_CMD_RUN_AT_CONSTRUCTOR_OF_SMART_PROCESS_THINGO_FAILS_LIST_VARIANT(replicaMounts, sudoMountCmdPrefix + currentReplica.first, sudoUmountCmdPrefix + currentReplica.first)
    }


    //prepare to backup, ensure sources and dest exist and that we can write to them
    RETURN_ONE_IF_DIR_IS_NOT_GOOD(videoSource)
    RETURN_ONE_IF_DIR_IS_NOT_GOOD(audioSource)
    RETURN_ONE_IF_VIDEOSOURCE_DIR_HAS_ANY_NONFILES_OR_NONH264FILES(videoSource) //handling sub-directories is a pain, so not going to allow it for now
    RETURN_ONE_IF_DIR_IS_NOT_GOOD(backupDestBase)
    //do the same, but for the replicas
    replicasIterator.toFront();
    while(replicasIterator.hasNext())
    {
        const QString &currentReplicasBaseDir = replicasIterator.next().second;
        RETURN_ONE_IF_DIR_IS_NOT_GOOD(currentReplicasBaseDir)
    }


    //prepate to backup, make the dest dirs, bailing out if any one of them fails to be made (or is already made)
    QString unixTimeInSecondsNow = QString::number(QDateTime::currentMSecsSinceEpoch()/1000);
    QString backupDestNewSubDir_WithSlashAppended = backupDestPrefix + unixTimeInSecondsNow + QDir::separator();;
    QString backupDest_WithSlashAppended = backupDestNewFolderForThisSync + backupDestNewSubDir_WithSlashAppended;
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
        QString replicaBackupDest_WithSlashAppended = removeTrailingSlashIfNeeded(replicasIterator.next().second) + destPathOnBackupAndReplicaMountPoints + backupDestNewSubDir_WithSlashAppended;
        RETURN_ONE_IF_MKDIR_AT_CONSTRUCTOR_OF_SMART_MKDIR_THINGO_FAILS_LIST_VARIANT(replicaDirs, replicaBackupDest_WithSlashAppended)
    }


    //since every mkdir succeeded, set "autoRemove" on the dirs to false (it would fail if there's content in it anyways, but still)
    newlyMadeBackupDestDir->AutoRemove = false;
    newlyMadeBackupDestAudioDir->AutoRemove = false;
    newlyMadeBackupDestVideoDir->AutoRemove = false;
    //do the same, but for replicas
    QListIterator<QSharedPointer<AutoRemovingDirMker> > replicaDirsIterator(replicaDirs);
    while(replicaDirsIterator.hasNext())
    {
        replicaDirsIterator.next()->AutoRemove = false;
    }

    //============DO THE ACTUAL BACKUP=========
    RETURN_ONE_IF_RSYNC_COPY_CMD_FAILS(audioSource, backupDestAudioDir_WithSlashAppended)
    //RETURN_ONE_IF_RSYNC_COPY_CMD_FAILS(videoSource, backupDestVideoDir_WithSlashAppended)
    RETURN_ONE_IF_ANY_FILE_IN_VIDEOSOURCE_FAILS_TO_COPYMUX(videoSource) //because ffprobe can't do raw h264, i mux and copy the video files (into mkv) at the same time. doing this allows me to change the hardcoded fps in the future without losing the fps for the vids made in the past


    //TODOreq: maybe run the recursive gpg signer at this point? it will detect most overwrites (we don't want to replicate overwrites), woot. recursive gpg signer serves doubly as a cheap af way to verify last modified timestamps for a hierarchy... but with the added bonus that it signs new files it sees :-D. the best protection would be to run recursive sign + verify here, so we KNOW there weren't any overwrites.... however that is slow as fuuuuuuuck to do every time....


    GIVE_WARNING_IF_SETTINGS_PERMISSIONS_TO_READ_ONLY_FAILS(backupDestAudioDir_WithSlashAppended)
    GIVE_WARNING_IF_SETTINGS_PERMISSIONS_TO_READ_ONLY_FAILS(backupDestVideoDir_WithSlashAppended)


    //replicate
    replicasIterator.toFront();
    QString hackyLoadBalancerChangingSourceHehForEachReplicaHeh = backupDest_WithSlashAppended; //at first backup dest is used as src, but then we use the first replica as the source, and so on. TODOoptional: a flag for a replica "don't use as source in load balancing" (usb [2.0] drives) -- atm I only have one replica usb drive and it's the very last replica anyways lolol
    while(replicasIterator.hasNext())
    {
        const QPair<QString,QString> &currentReplica = replicasIterator.next();
        QString replicaDestWithSlashAppended = appendSlashIfNeeded(currentReplica.second);
        RETURN_ONE_IF_RSYNC_COPY_CMD_FAILS(hackyLoadBalancerChangingSourceHehForEachReplicaHeh, replicaDestWithSlashAppended)
        GIVE_WARNING_IF_SETTINGS_PERMISSIONS_TO_READ_ONLY_FAILS(replicaDestWithSlashAppended)
        hackyLoadBalancerChangingSourceHehForEachReplicaHeh = replicaDestWithSlashAppended;
    }

    //TODOblah: gpg sign/verify/ANYTHING here?

    replicaMounts.clear(); //unmounts all replicas :)


    //delete sources (ffffffff one typo and I'm dead (is why I exit on ANY error))
    RETURN_ONE_IF_RECURSIVE_DELETE_OR_REMAKE_FAILS(audioSource)
    RETURN_ONE_IF_RECURSIVE_DELETE_OR_REMAKE_FAILS(videoSource)


    //unmount sources
    audioSourceMount.reset();
    videoSourceMount.reset();


    //mux
    //oh boy, the hardest part (by far) -- might as well go into OO mode since the muxing part might be re-used and I'd been planning on doing it in Qt anyways...
#if 0

#replicate to the usb in temp/dist dir for Morning script (TODOreq: delete these at the end of Morning script (or the beginning of this Night script, when we see our usb temp/dist dir was deleted by morning))

#umount

#endif

    return 0;
}
