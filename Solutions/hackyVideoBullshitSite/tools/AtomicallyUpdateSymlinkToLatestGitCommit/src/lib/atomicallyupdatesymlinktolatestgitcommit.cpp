#include "atomicallyupdatesymlinktolatestgitcommit.h"

#include <QFileInfo>
#include <QStringList>
#include <QDateTime>
#include <QDir>

#define AtomicallyUpdateSymlinkToLatestGitCommit_DEFAULT_GIT_PATH "/usr/bin/git"
#define AtomicallyUpdateSymlinkToLatestGitCommit_DEFAULT_MV_PATH "/bin/mv" //QFile::rename does not overwrite, even though QSaveFile does internally :(. /jelly

AtomicallyUpdateSymlinkToLatestGitCommit::AtomicallyUpdateSymlinkToLatestGitCommit(QObject *parent)
    : QObject(parent)
    , m_GitPath(AtomicallyUpdateSymlinkToLatestGitCommit_DEFAULT_GIT_PATH)
    , m_MvPath(AtomicallyUpdateSymlinkToLatestGitCommit_DEFAULT_MV_PATH)
    , m_Process(new QProcess(this))
{
    m_Process->setProcessChannelMode(QProcess::MergedChannels);
}
bool AtomicallyUpdateSymlinkToLatestGitCommit::atomicallyUpdateSymlinkToLatestGitCommitPrivate(const QString &cloneSrcUrl, const QString &symlinkToAtomicallyUpdate, const QString &tempDirForBothCloneDestinationsAndSymlinkPreparation)
{
    QFileInfo gitFileInfo(m_GitPath);
    if(!gitFileInfo.isExecutable())
    {
        emit e("'" + m_GitPath + "'' is not executable");
        return false;
    }
    QFileInfo mvFileInfo(m_MvPath);
    if(!mvFileInfo.isExecutable())
    {
        emit e("'" + m_MvPath + "' is not executable");
        return false;
    }
    QString tempDirForBothCloneDestinationsAndSymlinkPreparationWithSlashAppended = appendSlashIfNeeded(tempDirForBothCloneDestinationsAndSymlinkPreparation);
    QString cloneDest = tempDirForBothCloneDestinationsAndSymlinkPreparationWithSlashAppended + "repoAt" + QString::number(QDateTime::currentMSecsSinceEpoch()/1000);
    if(!gitCloneWithDepth1(cloneSrcUrl, cloneDest))
        return false;
    QFileInfo symlinkToAtomicallyUpdateFileInfo(symlinkToAtomicallyUpdate);
    QString symlinkTargetToMaybeDeleteLater; //empty string = nothing to delete later
    QString canonicalFilePath = symlinkToAtomicallyUpdateFileInfo.canonicalFilePath();
    if(symlinkToAtomicallyUpdateFileInfo.isSymLink() && QFile::exists(canonicalFilePath))
    {
        symlinkTargetToMaybeDeleteLater = canonicalFilePath;
    }
    QString tempSymbolicLink(tempDirForBothCloneDestinationsAndSymlinkPreparationWithSlashAppended + symlinkToAtomicallyUpdateFileInfo.fileName());

    //cleanup previous crashes (otherwise link will fail)
    QFile::remove(tempSymbolicLink);

    if(!QFile::link(cloneDest, tempSymbolicLink))
    {
        emit e("failed to create symlink (src = '" + cloneDest + "', link = '" + tempSymbolicLink + "')");
        //TODOreq: delete the depth 1 clone that is now never going to be used (or: periodically (cron) run a script that deletes all but the newest /repoAt<timestamp>)
        return false;
    }
    QString canonicalPath(appendSlashIfNeeded(symlinkToAtomicallyUpdateFileInfo.canonicalPath()));
    if(!atomicMoveOverwritingDestinationPlx(tempSymbolicLink, canonicalPath))
        return false;

    //delete previously linked to depth 1 clone
    if(!symlinkTargetToMaybeDeleteLater.isEmpty())
    {
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
bool AtomicallyUpdateSymlinkToLatestGitCommit::gitCloneWithDepth1(const QString &srcRepoAbsolutePath, const QString &destRepoAbsolutePath)
{
    QStringList gitCloneArguments;
    gitCloneArguments << "clone" << "--depth=1" << srcRepoAbsolutePath << destRepoAbsolutePath;
    m_Process->start(m_GitPath, gitCloneArguments);
    if(!m_Process->waitForStarted())
    {
        emit e("failed to start git clone executable");
        return false;
    }
    if(!m_Process->waitForFinished(-1 /*1000*60*60 1 hour clone time max lol*/))
    {
        emit e("git clone executable failed to finish");
        return false;
    }
    QString gitCloneResultString = m_Process->readAll();
    //TODOoptional: we could probably do basic 'contains' verification here, but return code checking is probably sufficient
    emit o(gitCloneResultString);
    int exitCode = m_Process->exitCode();
    if(exitCode != 0)
    {
        emit e("git clone exit code was not zero");
        return false;
    }
    if(gitCloneResultString.trimmed().isEmpty())
    {
        emit e("git clone didn't say anything, so there was probably an error");
        return false;
    }
    return true;
}
bool AtomicallyUpdateSymlinkToLatestGitCommit::atomicMoveOverwritingDestinationPlx(const QString &srcPath, const QString &destPath)
{
    QStringList atmoicMoveOverwritingDestinationArguments;
    atmoicMoveOverwritingDestinationArguments << "-f" /* I didn't need -f in my testing, but fuck it maybe it will alleviate obscure/subtle permissions bugs */ << srcPath << destPath;
    m_Process->start(m_MvPath, atmoicMoveOverwritingDestinationArguments);
    if(!m_Process->waitForStarted())
    {
        emit e("failed to start mv executable");
        return false;
    }
    if(!m_Process->waitForFinished())
    {
        emit e("mv executable failed to finish");
        return false;
    }
    QString mvResultString = m_Process->readAll();
    int exitCode = m_Process->exitCode();
    if(exitCode != 0)
    {
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
void AtomicallyUpdateSymlinkToLatestGitCommit::setMvPath(const QString &mvPath)
{
    m_MvPath = mvPath;
}
//symlinkToAtomicallyUpdate MUST NOT be in the root of tempDirForBothCloneDestinationsAndSymlinkPreparation
void AtomicallyUpdateSymlinkToLatestGitCommit::atomicallyUpdateSymlinkToLatestGitCommit(const QString &cloneSrcUrl, const QString &symlinkToAtomicallyUpdate, const QString &tempDirForBothCloneDestinationsAndSymlinkPreparation)
{
    emit symlinkAtomicallyUpdatedToLatestGitCommit(atomicallyUpdateSymlinkToLatestGitCommitPrivate(cloneSrcUrl, symlinkToAtomicallyUpdate, tempDirForBothCloneDestinationsAndSymlinkPreparation));
}
