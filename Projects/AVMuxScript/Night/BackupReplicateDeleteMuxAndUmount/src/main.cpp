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
#define backupDestPrefix backupDestBase "/daySyncedAt-"
#define backupDestAudioSubDir "a"
#define backupDestVideoSubDir "v"

#if 0 //corresponding fstab parts
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
    if(process.exitCode() != 0 || process.exitStatus() != QProcess::NormalExit)
    {
        DUMP_PROCESS_OUTPUT
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

#define mountCmdPrefix "mount -v "
#define sudoMountCmdPrefix "sudo " mountCmdPrefix
#define umountCmdPrefix "umount -v "
#define sudoUmountCmdPrefix "sudo " umountCmdPrefix

//i had specified partial, but took it out in favor of backup (because I wouldn't want to backup a partial lol
#define rsyncCmdPrefix "rsync -avhh --backup --progress --suffix=~accidentallyOverwrittenDuringSyncAt-"
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

#define RETURN_ONE_IF_MKPATH_FAILS(pathToMk) \
if(!backupDestDir.mkpath(pathToMk)) \
{ \
    qDebug() << "failed to make dir:" << pathToMk; \
    return 1; \
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

#define RETURN_ONE_IF_RECURSIVE_DELETE_OR_REMAKE_FAILS(videoSource) \
{ \
    QDir theDir(dirToDeleteAndRemake); \
    if(!theDir.removeRecursively()) \
    { \
        qDebug() << "failed to remove recursively:" << dirToDeleteAndRemake; \
        return 1; \
    } \
    if(!theDir.mkpath(dirToDeleteAndRemake)) \
    { \
        qDebug() << "failed to re-create dir just after recursively deleting it:" << dirToDeleteAndRemake; \
        return 1; \
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

int main(int argc, char *argv[]) //TODOoptional: "minNumReplicas" cli arg (hardcoded list of replicas (or hell, provided via args even). we TRY to replicate to all of them (maybe a hardcoded max + randomization of which we select), but error out before even starting if at least minNumReplicas can't be mounted/replicated-to)
{
    QCoreApplication a(argc, argv);
    return 1;

    //mount all -- TODOreq: ntfs drives need sudo xD

    //bool oneMountFailed = false;
    //if(!runProcess("mount -v " videoSourceMountPoint))
    //    oneMountFailed = true;

    RETURN_ONE_IF_CMD_RUN_AT_CONSTRUCTOR_OF_SMART_PROCESS_THINGO_FAILS(audioSourceMount, mountCmdPrefix audioSourceMountPoint, umountCmdPrefix audioSourceMountPoint)
    RETURN_ONE_IF_CMD_RUN_AT_CONSTRUCTOR_OF_SMART_PROCESS_THINGO_FAILS(videoSourceMount, mountCmdPrefix videoSourceMountPoint, umountCmdPrefix videoSourceMountPoint)
    RETURN_ONE_IF_CMD_RUN_AT_CONSTRUCTOR_OF_SMART_PROCESS_THINGO_FAILS(backupMount, sudoMountCmdPrefix backupMountPoint, sudoUmountCmdPrefix backupMountPoint)

    //TODOreq: make sure replicate drive[s] can mount here? leave them mounted during backup? idfk
    QList<QPair<QString /* replica mount point */, QString /* replica base dir */> > replicas;
    allReplicas
    QListIterator<QPair<QString, QString> > it(replicas);
    QList<QSharedPointer<BeforeAndAfterProcessRunner> > replicaMounts;
    while(it.hasNext())
    {
        const QPair<QString,QString> &currentReplica = it.next();
        //atm all my replicas are ntfs, so they require sudo... but eh i shouldn't issue it if it's not needed (and I'm considering switching every drive I have to ext4 because fuck that other OS :-P)
        RETURN_ONE_IF_CMD_RUN_AT_CONSTRUCTOR_OF_SMART_PROCESS_THINGO_FAILS_LIST_VARIANT(replicaMounts, sudoMountCmdPrefix + currentReplica.first, sudoUmountCmdPrefix + currentReplica.first)
    }


    //prepare to backup, ensure sources and dest exist and that we can write to them
    RETURN_ONE_IF_DIR_IS_NOT_GOOD(videoSource)
    RETURN_ONE_IF_DIR_IS_NOT_GOOD(audioSource)
    RETURN_ONE_IF_DIR_IS_NOT_GOOD(backupDestBase)

    //backup

    QString unixTimeInSecondsNow = QString::number(QDateTime::currentMSecsSinceEpoch()/1000);
    QString backupDest_WithSlashAppended = backupDestPrefix + unixTimeInSecondsNow + QDir::separator();
    QDir backupDestDir(backupDest_WithSlashAppended);
    if(backupDestDir.exists())
    {
        qDebug() << "dir we wanted to make and backup into already exists:" << backupDest_WithSlashAppended;
        return 1;
    }

    //prepare to backup, make destination directories
    RETURN_ONE_IF_MKPATH_FAILS(backupDest_WithSlashAppended) //absolute path
    RETURN_ONE_IF_MKPATH_FAILS(backupDestAudioSubDir) //relative path/sub-dir
    RETURN_ONE_IF_MKPATH_FAILS(backupDestVideoSubDir) //relative path/sub-dir

    //============DO THE ACTUAL BACKUP=========
    QString backupDestAudioDir_WithSlashAppended(backupDest_WithSlashAppended + backupDestAudioSubDir + QDir::separator());
    RETURN_ONE_IF_RSYNC_COPY_CMD_FAILS(audioSource, backupDestAudioDir_WithSlashAppended)
    QString backupDestVideoDir_WithSlashAppended(backupDest_WithSlashAppended + backupDestVideoSubDir + QDir::separator());
    RETURN_ONE_IF_RSYNC_COPY_CMD_FAILS(videoSource, backupDestVideoDir_WithSlashAppended)


    //TODOreq: maybe run the recursive gpg signer at this point? it will detect most overwrites (we don't want to replicate overwrites), woot. recursive gpg signer serves doubly as a cheap af way to verify last modified timestamps for a hierarchy... but with the added bonus that it signs new files it sees :-D. the best protection would be to run recursive sign + verify here, so we KNOW there weren't any overwrites.... however that is slow as fuuuuuuuck to do every time....


    GIVE_WARNING_IF_SETTINGS_PERMISSIONS_TO_READ_ONLY_FAILS(backupDestAudioDir_WithSlashAppended)
    GIVE_WARNING_IF_SETTINGS_PERMISSIONS_TO_READ_ONLY_FAILS(backupDestVideoDir_WithSlashAppended)


    //replicate
    it.toFront();
    QString hackyLoadBalancerChangingSourceHehForEachReplicaHeh = backupDest_WithSlashAppended; //at first backup dest is used as src, but then we use the first replica as the source, and so on. TODOoptional: a flag for a replica "don't use as source in load balancing" (usb [2.0] drives) -- atm I only have one replica usb drive and it's the very last replica anyways lolol
    while(it.hasNext())
    {
        const QPair<QString,QString> &currentReplica = it.next();
        QString replicaDestWithSlashAppended = appendSlashIfNeeded(currentReplica.second);
        RETURN_ONE_IF_RSYNC_COPY_CMD_FAILS(hackyLoadBalancerChangingSourceHehForEachReplicaHeh, replicaDestWithSlashAppended)
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
