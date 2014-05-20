#include "atomicallyupdatesymlinktolatestgitcommit.h"

#include <QFileInfo>
#include <QStringList>
#include <QDateTime>
#include <QDir>
#include <QProcess>

#define AtomicallyUpdateSymlinkToLatestGitCommit_DEFAULT_GIT_PATH "/usr/bin/git"
#define AtomicallyUpdateSymlinkToLatestGitCommit_DEFAULT_TAR_PATH "/bin/tar"
#define AtomicallyUpdateSymlinkToLatestGitCommit_DEFAULT_MV_PATH "/bin/mv" //QFile::rename does not overwrite, even though QSaveFile does internally :(. /jelly

//TODOoptional: could allow the user to supply the command that populates the nextDir (but should still suggest/demonstrate archive/clone usage for copy/pasting)
//TODOoptional: if I use this for /binary/ as well, the /Videos/Airborne symlink needs to be created inside as well. this is a custom extra step that should be optional or specifiable or fuck idfk

AtomicallyUpdateSymlinkToLatestGitCommit::AtomicallyUpdateSymlinkToLatestGitCommit(QObject *parent)
    : QObject(parent)
    , m_GitPath(AtomicallyUpdateSymlinkToLatestGitCommit_DEFAULT_GIT_PATH)
    , m_TarPath(AtomicallyUpdateSymlinkToLatestGitCommit_DEFAULT_TAR_PATH)
    , m_MvPath(AtomicallyUpdateSymlinkToLatestGitCommit_DEFAULT_MV_PATH)
    , m_MvProcess(new QProcess(this))
{ }
bool AtomicallyUpdateSymlinkToLatestGitCommit::atomicallyUpdateSymlinkToLatestGitCommitPrivate(const QString &cloneSrcUrl, const QString &symlinkToAtomicallyUpdate, const QString &tempDirForBothCloneDestinationsAndSymlinkPreparation)
{
    QFileInfo gitFileInfo(m_GitPath);
    if(!gitFileInfo.isExecutable())
    {
        emit e("'" + m_GitPath + "'' is not executable");
        return false;
    }
    QFileInfo tarFileInfo(m_TarPath);
    if(!tarFileInfo.isExecutable())
    {
        emit e("'" + m_TarPath + "'' is not executable");
        return false;
    }
    QFileInfo mvFileInfo(m_MvPath);
    if(!mvFileInfo.isExecutable())
    {
        emit e("'" + m_MvPath + "' is not executable");
        return false;
    }
    QString tempDirForBothCloneDestinationsAndSymlinkPreparationWithSlashAppended = appendSlashIfNeeded(tempDirForBothCloneDestinationsAndSymlinkPreparation);
    QString latestCommitArchiveDest = tempDirForBothCloneDestinationsAndSymlinkPreparationWithSlashAppended + "repoAt" + QString::number(QDateTime::currentMSecsSinceEpoch()/1000) + "/";
    QDir dumbInstance(latestCommitArchiveDest);
    if(!dumbInstance.mkpath(latestCommitArchiveDest))
    {
        emit e("failed to make dir: '" + latestCommitArchiveDest + "'");
        return false;
    }
    if(!gitArchiveAndUntarLatestCommit(cloneSrcUrl, latestCommitArchiveDest)) //this used to be git clone --depth=1, but that is twice (minimum!) as expensive for binaries! //TODOoptimization: git archive switches to write files skipping tar stage..
        return false;
    QFileInfo symlinkToAtomicallyUpdateFileInfo(symlinkToAtomicallyUpdate);
    QString symlinkTargetToMaybeDeleteLater; //empty string = nothing to delete later
    QString canonicalFilePath = symlinkToAtomicallyUpdateFileInfo.canonicalFilePath();
    if(symlinkToAtomicallyUpdateFileInfo.isSymLink() && QFile::exists(canonicalFilePath))
    {
        symlinkTargetToMaybeDeleteLater = appendSlashIfNeeded(canonicalFilePath);
    }
    QString tempSymbolicLink(tempDirForBothCloneDestinationsAndSymlinkPreparationWithSlashAppended + symlinkToAtomicallyUpdateFileInfo.fileName());

    //cleanup previous crashes (otherwise link will fail)
    QFile::remove(tempSymbolicLink);

    if(!QFile::link(latestCommitArchiveDest, tempSymbolicLink))
    {
        emit e("failed to create symlink (src = '" + latestCommitArchiveDest + "', link = '" + tempSymbolicLink + "')");
        //TODOreq: delete the depth 1 clone that is now never going to be used (or: periodically (cron) run a script that deletes all but the newest /repoAt<timestamp>)
        return false;
    }
    QString directoryContainingSymlinkToAtomicallyUpdate(appendSlashIfNeeded(QDir::cleanPath(symlinkToAtomicallyUpdateFileInfo.absolutePath())));
    if(!atomicMoveOverwritingDestinationPlx(tempSymbolicLink, directoryContainingSymlinkToAtomicallyUpdate))
        return false;

    //delete previously linked-to depth 1 clone
    if(!symlinkTargetToMaybeDeleteLater.isEmpty())
    {
        QFile::remove(symlinkTargetToMaybeDeleteLater + ".lastModifiedTimestamps"); //dgaf if fails (not exist etc), since it will be deleted momentarily anyways. we delete it before the other files/dirs just because HVBS is watching it with a QFSW, so it's merely an optimization. TODOoptional: allow specifying filename, call it... "most important file to delete first because it's qfs watched" or some such (idklol)

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        //qt 4.x
        QFileInfo rmExe("/bin/rm"); //TODOportability: does windows even have a recursive delete ability? been so long i can't remember (that's not a bad thing (*makes note in calendar and celebrates it yearly*))!). better question, why the fuck wasn't recursivelyRemove backported to 4.x? userland qt can obviously do it (i'm lazy), why only in 5.x??
        bool successFullyDeleted = false;
        if(rmExe.isExecutable())
        {
            QStringList rmArgs;
            rmArgs << "-rf" << symlinkTargetToMaybeDeleteLater;
            m_Process->start(rmExe.canonicalFilePath(), QIODevice::ReadOnly);
            if(m_Process->waitForStarted())
            {
                if(m_Process->waitForFinished(-1 /*1000*60*30 30 mins because could get large*/))
                {
                    QString recursiveRemoveResultString = m_Process->readAll();
                    if(m_Process->exitCode() == 0)
                    {
                        if(recursiveRemoveResultString.trimmed().isEmpty())
                        {
                            successFullyDeleted = true;
                        }
                        else
                        {
                            emit e("recursive remove said something, so there was probably an error: '" + recursiveRemoveResultString + "'");
                        }
                    }
                    else
                    {
                        emit e("git clone exit code was not zero: '" + recursiveRemoveResultString + "'");
                    }
                }
                else
                {
                    emit e("rm failed to finish");
                }
            }
            else
            {
                emit e("rm failed to start");
            }
        }

        if(!successFullyDeleted)
        {
            emit e("failed to delete directory: '" + symlinkTargetToMaybeDeleteLater + "'. you must do so manually otherwise tons of space will wasted on depth 1 clones");
            //non-fatal, no return false;
        }
#else
        //qt 5.x
        QDir oldDepth1clone(symlinkTargetToMaybeDeleteLater);
        if(!oldDepth1clone.removeRecursively())
        {
            emit e("failed to recursively remove '" + symlinkTargetToMaybeDeleteLater + "'. you must do so manually otherwise tons of space will wasted on depth 1 clones");
            //non-fatal, no return false;
        }
#endif
    }
    return true;
}
bool AtomicallyUpdateSymlinkToLatestGitCommit::gitArchiveAndUntarLatestCommit(const QString &srcRepoUrl, const QString &destArchivePath)
{
    QStringList tarArgs;
    tarArgs << "-x" << "-C" << destArchivePath;
    QProcess tarProcess(this); //local so i can just return false; /lazy (+smart)
    QProcess gitProcess(this);
    gitProcess.setStandardOutputProcess(&tarProcess);
    tarProcess.start(m_TarPath, tarArgs, QIODevice::WriteOnly);
    QStringList gitArchiveArgs;
    QString remoteSrcArg = "--remote=" + srcRepoUrl;
    gitArchiveArgs << "archive" << remoteSrcArg <<  "master"; //TODOoptional: allow specifying branch other than master
    gitProcess.start(m_GitPath, gitArchiveArgs, QIODevice::ReadOnly);
    if(!tarProcess.waitForStarted())
    {
        emit e("failed to start tar executable");
        return false;
    }
    if(!gitProcess.waitForStarted())
    {
        emit e("failed to start git archive executable");
        return false;
    }
#if 0 //derp qt saves day again
    if(!gitProcess.waitForReadyRead(-1))
    {
        emit e("git process did not write to stdout or error or something");
        return false;
    }
    QByteArray bufferBA;
    //bufferBA.reserve(AtomicallyUpdateSymlinkToLatestGitCommit_GIT_READ_BUFFER_SIZE_MAX);
    qint64 bytesAvail = gitProcess.bytesAvailable();
    while(bytesAvail > 0)
    {
        qint64 bytesToRead = qMin(bytesAvail, AtomicallyUpdateSymlinkToLatestGitCommit_GIT_READ_BUFFER_SIZE_MAX);
        bufferBA = gitProcess.read(bytesToRead);
        tarProcess.write(bufferBA);
        bytesAvail = gitProcess.bytesAvailable();
    }
    gitProcess.waitForFinished()
#endif
    if(!gitProcess.waitForFinished(-1 /*1000*60*60 1 hour clone time max lol*/))
    {
        emit e("git archive executable failed to finish");
        return false;
    }
    if(!tarProcess.waitForFinished(-1))
    {
        emit e("tar executable failed to finish");
        return false;
    }
    int tarExitCode = tarProcess.exitCode();
    if(tarExitCode != 0)
    {
        QByteArray stdErrBA = tarProcess.readAllStandardError();
        QString stdErr(stdErrBA);
        emit e("tar stderr said: " + stdErr);
        emit e("tar exit code was not zero");
        return false;
    }
    int gitExitCode = gitProcess.exitCode();
    if(gitExitCode != 0)
    {
        QByteArray stdErrBA = gitProcess.readAllStandardError();
        QString stdErr(stdErrBA);
        emit e("git stderr said: " + stdErr);
        emit e("git archive exit code was not zero");
        return false;
    }
    return true;
}
bool AtomicallyUpdateSymlinkToLatestGitCommit::atomicMoveOverwritingDestinationPlx(const QString &srcPath, const QString &destPath)
{
    QStringList atmoicMoveOverwritingDestinationArguments;
    atmoicMoveOverwritingDestinationArguments << "-f" /* I didn't need -f in my testing, but fuck it maybe it will alleviate obscure/subtle permissions bugs */ << srcPath << destPath;
    m_MvProcess->start(m_MvPath, atmoicMoveOverwritingDestinationArguments);
    if(!m_MvProcess->waitForStarted())
    {
        emit e("failed to start mv executable");
        return false;
    }
    if(!m_MvProcess->waitForFinished())
    {
        emit e("mv executable failed to finish");
        return false;
    }
    QString mvResultString = m_MvProcess->readAll();
    int exitCode = m_MvProcess->exitCode();
    if(exitCode != 0)
    {
        QByteArray stdErrBA = m_MvProcess->readAllStandardError();
        QString stdErr(stdErrBA);
        emit e("mv stderr said: " + stdErr);
        emit e("mv exit code was not zero: '" + mvResultString + "'");
        return false;
    }
    if(!mvResultString.trimmed().isEmpty())
    {
        emit e("mv said something, so there was probably an error: '" + mvResultString + "'");
        return false;
    }
    return true;
}
void AtomicallyUpdateSymlinkToLatestGitCommit::setGitPath(const QString &gitPath)
{
    m_GitPath = gitPath;
}
void AtomicallyUpdateSymlinkToLatestGitCommit::setTarPath(const QString &tarPath)
{
    m_TarPath = tarPath;
}
void AtomicallyUpdateSymlinkToLatestGitCommit::setMvPath(const QString &mvPath)
{
    m_MvPath = mvPath;
}
//symlinkToAtomicallyUpdate MUST NOT be in the root of tempDirForBothCloneDestinationsAndSymlinkPreparation
void AtomicallyUpdateSymlinkToLatestGitCommit::atomicallyUpdateSymlinkToLatestGitCommit(const QString &cloneSrcUrl, const QString &symlinkToAtomicallyUpdate, const QString &tempDirForBothCloneDestinationsAndSymlinkPreparation)
{
    emit symlinkAtomicallyUpdatedToLatestGitCommit(atomicallyUpdateSymlinkToLatestGitCommitPrivate(cloneSrcUrl, symlinkToAtomicallyUpdate, tempDirForBothCloneDestinationsAndSymlinkPreparation));
}
