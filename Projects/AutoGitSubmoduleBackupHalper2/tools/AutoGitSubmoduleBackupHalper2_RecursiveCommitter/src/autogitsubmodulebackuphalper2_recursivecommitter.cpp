#include "autogitsubmodulebackuphalper2_recursivecommitter.h"

#include <QDirIterator>
#include <QFileInfo>

AutoGitSubmoduleBackupHalper2_RecursiveCommitter::AutoGitSubmoduleBackupHalper2_RecursiveCommitter(QObject *parent)
    : QObject(parent)
{ }
bool AutoGitSubmoduleBackupHalper2_RecursiveCommitter::recursivelyCommitActual(const QString &sourceDirWhichIsMyMonolithicAutobackupDir, const QString &destDirWhichContainsAsuperprojectAndAboatloadOfSha1hexIdentifiedBareGitSubmodules)
{
#if 1 //actually we only need to check for the LACK OF dirs existing, then commit instead of recursing INTO the dir(s). we also commit AFTER recursing into the dirs~. blah my head hurts and I don't have confidence in my git submodule skillz. one way of putting it is: recurse into all dirs first, then when only files left: commit! (but commit intelligently, submodule-aware!). or as "Skeith" of DDNet legend once put it to me so elegantly: "commit you pussy". he was right.
    QDirIterator dirIt(sourceDirWhichIsMyMonolithicAutobackupDir /*hmm we don't need DOUBLE recursion! one or the other works: ,QDirIterator::Subdirectories*/); //TODOoptimization: QDirIterator::Subdirectories might scale better?
    while(dirIt.hasNext())
    {
        dirIt.next();
        const QFileInfo &fileInfo = dirIt.fileInfo();
        if(fileInfo.isDir())
        {
            QString subDir = appendSlashIfNecessary(sourceDirWhichIsMyMonolithicAutobackupDir + dirIt.fileName());
            if(!recursivelyCommit(subDir, destDirWhichContainsAsuperprojectAndAboatloadOfSha1hexIdentifiedBareGitSubmodules))
                return false;
        }
        else if(fileInfo.isFile())
        {
            //commit after setting up GIT_DIR/etc. I guess we could just WAIT until the dirIterator.hasNext() return false, at which point we KNOW we [recursively] processed all dirs, [and did nothing for the files because that's this line of code] -- or maybe we need to "git add fileInfo.fileName()" since the dirs (submodules) have to be added differently, ie as submodules! I need to research what "git add ." does when submodules are involved kek
        }
        else
        {
            emit e("neither dir nor file found: " + fileInfo.absoluteFilePath());
            return false;
        }
    }
#endif

    //done processing this dir. git commit (submodule aware. all dirs are submodules) after settigns up GIT_DIR/etc

    //TODOreq: the submodules (dirs) probably have to be "setup" before git add . && git commit (below)
    //wtf does "setup" mean, I have no idea but I'm tryin to look up the git submodules tutorial as we speak~
    //Note: this code is, must, be the same for "the superproject" (top-level containing only dirs in theory) and "the subdirs" (which contain subdirs (which are submodules (which are subdirs)) and files) -- the code to  "process" both is the same: here/now

    //TODOreq:
    if(!gitAddAll(sourceDirWhichIsMyMonolithicAutobackupDir, destDirWhichContainsAsuperprojectAndAboatloadOfSha1hexIdentifiedBareGitSubmodules))
        return false;
    if(!gitCommit(sourceDirWhichIsMyMonolithicAutobackupDir, destDirWhichContainsAsuperprojectAndAboatloadOfSha1hexIdentifiedBareGitSubmodules))
        return false;

    return true;
}
void AutoGitSubmoduleBackupHalper2_RecursiveCommitter::recursivelyCommit(const QString &sourceDirWhichIsMyMonolithicAutobackupDir, const QString &destDirWhichContainsAsuperprojectAndAboatloadOfSha1hexIdentifiedBareGitSubmodules)
{
    emit recursivelyCommitFinished(recursivelyCommitActual(appendSlashIfNecessary(sourceDirWhichIsMyMonolithicAutobackupDir), appendSlashIfNecessary(destDirWhichContainsAsuperprojectAndAboatloadOfSha1hexIdentifiedBareGitSubmodules)));
}
