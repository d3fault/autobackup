#include "gitunrollrerollcensorshipmachine.h"

GitUnrollRerollCensorshipMachine::GitUnrollRerollCensorshipMachine(QObject *parent) :
    QObject(parent), m_UnusedFilanameNonce(0)
{
    connect(&m_GitHelper, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
QString GitUnrollRerollCensorshipMachine::getUnusedFilename(QDir dirToGetUnusedFilenameIn)
{
    QString potentialUnusedFilename;
    do
    {
        QString currentDateTime = QDateTime::currentDateTime().toString() + QString::number(++m_UnusedFilanameNonce);
        QByteArray blah(currentDateTime.toLatin1());
        QByteArray md5sum = QCryptographicHash::hash(blah, QCryptographicHash::Md5);
        potentialUnusedFilename = md5sum.toHex();
        potentialUnusedFilename = potentialUnusedFilename.left(10);
    } while(dirToGetUnusedFilenameIn.exists(potentialUnusedFilename));

    return potentialUnusedFilename;
}
QString GitUnrollRerollCensorshipMachine::appendSlashIfNeeded(QString input)
{
    if(!input.endsWith("/"))
    {
        input.append("/");
    }
    return input;
}
void GitUnrollRerollCensorshipMachine::unrollRerollGitRepoCensoringAtEachCommit(QString filePathToListOfFilepathsToCensor, QString absoluteSourceGitDirToCensor, QString absoluteDestinationGitDirCensored, QString absoluteWorkingDir)
{
    /*

    load list of files to censor

    clone src pre-censored repo to working dir: git --work-tree=/path/to/working/dir/ clone file:///path/to/src/repo/to/censor dotGitFolderLooksBareButIsnt //make sure cwd is somewhere on working dir
    ^^the dir at the end of the clone command should not yet exist, and neither should the work-tree
    ^^^just be sure the cwd when running is NOT the "actual working dir"

    init destinationGitDirCensored -- git --work-tree=/path/to/working/dir/ --git-dir=/path/to/dest/repo/censored/ init (both dirs must exist)

      foreach(git commit from beginning)
      {
        git --work-tree=/path/to/working/dir/ --git-dir=/path/to/dotGitFolderLooksBareButIsnt checkout <commit-id>

        delete/ensure-deleted every file to censor

        git add -- cd into actual working dir, then: git --work-tree=/path/to/working/dir/ --git-dir=/path/to/dest/repo/censored/ add -v .

        commit -- git --work-tree=/path/to/working/dir/ --git-dir=/path/to/dest/repo/censored/ commit -am --date=%DATE% "%COMMIT_MSG%"

      }
      */

    //1.0 no hacks
    QFile fileWithListOfFilePathsToCensor(filePathToListOfFilepathsToCensor);
    if(!fileWithListOfFilePathsToCensor.exists())
    {
        emit d("file does not exist: " + filePathToListOfFilepathsToCensor);
        return;
    }
    if(!QFile::exists(absoluteSourceGitDirToCensor))
    {
        emit d("folder does not exist: " + absoluteSourceGitDirToCensor);
        return;
    }
    if(!QFile::exists(absoluteDestinationGitDirCensored))
    {
        emit d("folder does not exist: " + absoluteDestinationGitDirCensored);
        return;
    }
    if(!QFile::exists(absoluteWorkingDir))
    {
        emit d("folder does not exist: " + absoluteWorkingDir);
        return;
    }
    absoluteSourceGitDirToCensor = appendSlashIfNeeded(absoluteSourceGitDirToCensor);
    absoluteDestinationGitDirCensored = appendSlashIfNeeded(absoluteDestinationGitDirCensored);
    absoluteWorkingDir = appendSlashIfNeeded(absoluteWorkingDir);

    QDir topWorkingDir(absoluteWorkingDir);
    QString actualWorkingSubDir = getUnusedFilename(topWorkingDir);
    QString absoluteActualWorkingDir = appendSlashIfNeeded(absoluteWorkingDir + actualWorkingSubDir);


    if(!fileWithListOfFilePathsToCensor.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("failed to open for reading: " + filePathToListOfFilepathsToCensor);
        return;
    }
    QList<EasyTreeHashItem*> *easyTreeHashItemListOfFilesToCensor = EasyTreeParser::parseEasyTreeAndReturnAsNewList(&fileWithListOfFilePathsToCensor);
    QStringList absoluteFilePathsToCensor;
    int easyTreeHashItemListOfFilesToCensorSize = easyTreeHashItemListOfFilesToCensor->size();
    for(int i = 0; i < easyTreeHashItemListOfFilesToCensorSize; ++i)
    {
        absoluteFilePathsToCensor << QString(absoluteActualWorkingDir + easyTreeHashItemListOfFilesToCensor->at(i)->relativeFilePath());
    }

    //CLONE REPO TO CENSOR

    QString censoredReposAssociatedDetachedGitSubDir = getUnusedFilename(topWorkingDir);
    QString absoluteCensoredSourceDirsAssociatedDetachedGitFolder = appendSlashIfNeeded(absoluteWorkingDir + censoredReposAssociatedDetachedGitSubDir);

#if 0 //i guess git clone makes these dirs :-/
    if(!topWorkingDir.mkdir(censoredReposAssociatedDetachedGitFolder))
    {
        emit d("failed to create sub working dir: " + absoluteCensoredReposAssociatedDetachedGitFolder);
        return;
    }
    if(!topWorkingDir.mkdir(actualWorkingDir))
    {
        emit d("failed to create sub working dir: " + absoluteActualWorkingDir);
        return;
    }
#endif

    if(!m_GitHelper.gitClone(absoluteSourceGitDirToCensor, absoluteCensoredSourceDirsAssociatedDetachedGitFolder, absoluteActualWorkingDir))
    {
        emit d("failed to clone the source directory to be censored");
        return;
    }
    //my mind exploded here when i realized that no, you should not return a bool from a slot because "cross thread calls" (signals/slots) mimic the cross network paradigm... and it would be fucking stupid if a computer you were talking to over the network error'd out which caused you to error out. still, wtf is the 'proper design' to not continue if the git clone fails!?!? saving and going to sleep
    //^who gives a shit. this is a hack. RpcGenerator and/or designEquals solve that problem, it's just that the proper solution is difficult to "keep track of the design/flow" in _code_. designEquals helps you respond properly to signals and all that proper yada jazz bullshit. I was tired was the main reason I stopped..

    //INIT CENSORED DESTINATION REPO

    //make git init censored repo dirs -- TODOreq: we're using a subfolder on the working dir to populate our censored destination repo throughout execution, but at the very end we need to do another "git clone" to populate the directory that the user requested

    QString tempCensoredDestinationAssociatedDetachedGitDir = getUnusedFilename(topWorkingDir);
    QString absoluteTempCensoredDestinationAssociatedDetachedGitDir = appendSlashIfNeeded(absoluteWorkingDir + tempCensoredDestinationAssociatedDetachedGitDir);

    emit d("here is where the result is for now because i'm a lazy cunt" + absoluteTempCensoredDestinationAssociatedDetachedGitDir);

    if(!topWorkingDir.mkdir(tempCensoredDestinationAssociatedDetachedGitDir))
    {
        emit d("failed to make sub working dir: " + absoluteTempCensoredDestinationAssociatedDetachedGitDir);
        return;
    }

    if(!m_GitHelper.gitInit(absoluteActualWorkingDir, absoluteTempCensoredDestinationAssociatedDetachedGitDir))
    {
        emit d("failed to init the [temp/working] censored destination directory");
        return;
    }

    //ITERATE GIT LOG HISTORY

    //gather git log
    QList<GitCommitIdTimestampAndMessage*> *allCommitSha1sFromGitLogCommand = new QList<GitCommitIdTimestampAndMessage*>();
    if(!m_GitHelper.gitLogReturningCommitIdsOnly(allCommitSha1sFromGitLogCommand, absoluteCensoredSourceDirsAssociatedDetachedGitFolder))
    {
        emit d("failed to run git log in order to iterate the history of the censored repo");
        return;
    }
    //iterate over the git log backwards, because we want to start from the beginning/oldest commit
    int censoredRepoCommitsCount = allCommitSha1sFromGitLogCommand->size(); //TODOreq: verify this commit count with our destination/censored commit count
    for(int i = censoredRepoCommitsCount-1; i > -1; --i) //TODOreq: verify not off by one error
    {
        GitCommitIdTimestampAndMessage *commitIdTimestampAndMessage = allCommitSha1sFromGitLogCommand->at(i);
        QString currentCommitId = commitIdTimestampAndMessage->commitId;

        //CHECKOUT

        if(!m_GitHelper.gitCheckout(currentCommitId, absoluteActualWorkingDir, absoluteCensoredSourceDirsAssociatedDetachedGitFolder))
        {
            emit d("failed to run git checkout for commitId: " + currentCommitId);
            return;
        }

        emit d("have now checked out: " + currentCommitId);

        //DELETE / ENSURE-DELETED list of filepaths

        foreach(QString absoluteFilePathToCensor, absoluteFilePathsToCensor)
        {
            if(QFile::exists(absoluteFilePathToCensor))
            {
                if(!QFile::remove(absoluteFilePathToCensor))
                {
                    emit d("failed to remove file: " + absoluteFilePathToCensor);
                    return;
                }
                emit d("---Deleted File: " + absoluteFilePathToCensor);
            }
        }

        //GIT ADD

        if(!m_GitHelper.gitAdd(absoluteActualWorkingDir, absoluteTempCensoredDestinationAssociatedDetachedGitDir))
        {
            emit d("failed to run git add");
            return;
        }

        //GIT COMMIT
        if(!m_GitHelper.gitCommit(absoluteActualWorkingDir, absoluteTempCensoredDestinationAssociatedDetachedGitDir, commitIdTimestampAndMessage->commitMessage, commitIdTimestampAndMessage->commitDate, "d3fault <d3fault@d3fault.d3fault>")) //TODOoptional: allow author to be specified or even parsed/re-used -- too many other hacks to do for me to give a shit
        {
            emit d("failed to run git commit");
            return;
        }
    }

    emit d("done iterating git commits");

    //CLONE TEMP CENSORED DESTINATION TO USER SUPPLIED DESTINATION


    //cleanup -- TODOreq: return;'ing anywhere above does not properly clean up! ffffff need me some scoped pointers
    int allCommitSha1sFromGitLogCommandCount = allCommitSha1sFromGitLogCommand->size();
    for(int i = 0; i < allCommitSha1sFromGitLogCommandCount; ++i)
    {
        GitCommitIdTimestampAndMessage *idTimestampAndMessage = allCommitSha1sFromGitLogCommand->at(i);
        delete idTimestampAndMessage;
    }
    delete allCommitSha1sFromGitLogCommand;

    int easyTreeHashItemListOfFilesToCensorCount = easyTreeHashItemListOfFilesToCensor->size();
    for(int i = 0; i < easyTreeHashItemListOfFilesToCensorCount; ++i)
    {
        EasyTreeHashItem *item = easyTreeHashItemListOfFilesToCensor->at(i);
        delete item;
    }
    delete easyTreeHashItemListOfFilesToCensor;

    //2.0 custom hacks
}
