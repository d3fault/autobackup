#include "quickdirtyautobackuphalperbusiness.h"

QuickDirtyAutoBackupHalperBusiness::QuickDirtyAutoBackupHalperBusiness(QObject *parent) :
    QObject(parent), m_StillUsingOurOwnAllocatedInternalList(true), m_PathToTruecryptBinary("/usr/bin/truecrypt"), m_PathToGitBinary("/usr/bin/git") /* TODOoptional: make binary path customizable/persisted. should be all that's needed to work on windows. well that and git binary specifying */, m_Process(0), m_ShutdownTimer(0), m_SecondsLeftUntilShutdown(DEFAULT_SHUTDOWN_SECONDS_COUNTDOWN_AMOUNT), m_ShuttingDown(false), m_MountedAsReadOnlyFlagForDismount(false)
{
    m_TypicalTruecryptArgs << "-t" << "--non-interactive" << "--verbose";
}
QuickDirtyAutoBackupHalperBusiness::~QuickDirtyAutoBackupHalperBusiness()
{
    //TODOmb: these were allocated on a different thread than the one that will be running the destructor? m_Process was allocated in start(), which is the first thing we call once on different thread. maybe i need an exitting() method that is called before QThread::quit(), QThread::wait(), and then the destructor is called god knows when but probably after both of those are done...?


    //delete m_Process;
    //delete m_ShutdownTimer;
    //^^for those two at least, i just added (this) as my parent so they should auto-destruct. FUCK THIS DOESN'T TELL ME ANYTHING BECAUSE DOESN'T THIS DESTRUCT ON THE GUI THREAD TOO????
}
void QuickDirtyAutoBackupHalperBusiness::readSettingsAndProbeMountStatus()
{
    //load from settings
    QSettings settings;
    int tcLocationArraySize = settings.beginReadArray(QString(TC_CONTAINERS_AND_PW_FILES_ARRAY_STRING));
    for(int i = 0; i < tcLocationArraySize; ++i)
    {
        settings.setArrayIndex(i);

        QString tcContainerPath(settings.value(QString(TC_CONTAINERS_PATH_STRING_KEY)).toString());
        QString tcPasswordFilePath(settings.value(QString(TC_PASSWORD_FILE_PATH_STRING_KEY)).toString());
        if((!tcContainerPath.isEmpty()) && (!tcPasswordFilePath.isEmpty()))
        {
            m_InternalTcContainerAndPasswordPathsList->append(qMakePair(tcContainerPath, tcPasswordFilePath));
        }
    }
    settings.endArray();

    //verify each exists etc
    if(!allContainerAndPwFilePathsAreValid(m_InternalTcContainerAndPasswordPathsList))
    {
        emit d("some of the paths in the settings weren't valid. check above errors... close/relaunch (or delete file: " + settings.fileName() + QString(") to start over"));
        return;
    }

    bool fucked = probeMountStatusOfInternalListAndEmitIfEitherAllMountedOrAllDismounted();

    if(fucked)
    {
        emit d("some errors regarding mounted/dismounted status of read in settings. perhaps some of the containers are mounted, some are dismounted. they need to all be either one or the other. fix it in the truecrypt gui and try again. read above errors");
        emit brokenStateDetected();
        return;
    }
    //TODOoptional: do a git status and send the results to the GUI. this basically just tells us what files we're working with. what we're about to commit. it will also help with writing the commit message...
}
void QuickDirtyAutoBackupHalperBusiness::scanForMountedRelevantContainers(QList<QPair<QString, QString> > *listOfRelevantContainers)
{
    int oldObjectsCount = m_ListOfMountedContainers->size();
    for(int i = 0; i < oldObjectsCount; ++i)
    {
        delete m_ListOfMountedContainers->at(i);
    }
    m_ListOfMountedContainers->clear();

    if(listOfRelevantContainers->size() == 0)
        return;

    QStringList listCommandArgs(m_TypicalTruecryptArgs);
    listCommandArgs << "-l";
    m_Process->start(m_PathToTruecryptBinary, listCommandArgs);
    if(!m_Process->waitForStarted())
    {
        emit d("failed to start truecrypt binary for details object population");
        emit brokenStateDetected();
        return;
    }
    if(!m_Process->waitForFinished())
    {
        emit d("truecrypt failed to finish during details object population");
        emit brokenStateDetected();
        return;
    }
    QString verboseListResultString(m_Process->readAll());

    emit d(QString("Truecrypt Details List Results: ") + verboseListResultString); //this is below the check for error above because we want to hide

    int errorStringCount = verboseListResultString.count("Error:", Qt::CaseInsensitive);
    if(errorStringCount == 1)
    {
        if(verboseListResultString.contains("No volumes mounted.", Qt::CaseInsensitive))
        {
            //'all dismounted' case
            emit d("we've detected via error that there are no volumes mounted. this is OK. ignore the above error, it is just debug");
            return;
        }
        else
        {
            emit d("truecrypt settings list returned some unexpected error");
            emit brokenStateDetected();
            return;
        }
    }
    else if(errorStringCount != 0)
    {
        emit d("got unexpected error count");
        emit brokenStateDetected();
        return;
    }
    else /*errorStringCount == 0*/  //<-- whitelist programming
    {
        QTextStream outputStream(&verboseListResultString, QIODevice::ReadOnly);
        bool getRelevantKeysBecauseWeSawTheWordVolume = false;
        QString thisLine;
        while(!outputStream.atEnd())
        {
            thisLine = outputStream.readLine();
            if(thisLine.isEmpty())
            {
                continue;
            }
            if(!thisLine.contains(":"))
            {
                continue;
            }
            QStringList keyValueSplit = thisLine.split(":", QString::SkipEmptyParts);
            if(keyValueSplit.size() != 2)
            {
                continue;
            }
            QString keyString = keyValueSplit.at(0).trimmed().toLower();
            QString valueString = keyValueSplit.at(1).trimmed();

            if(keyString == "volume")
            {
                int listOfContainersCount = listOfRelevantContainers->size();
                bool breakOut = true; //needed because we're IN a for loop as we analyze whether to break out rofl. fuck it
                for(int k = 0; k < listOfContainersCount; ++k)
                {
                    if(listOfRelevantContainers->at(k).first == valueString)
                    {
                        getRelevantKeysBecauseWeSawTheWordVolume = true;
                        breakOut = false;

                        TruecryptVerboseListMountPointDetailsObject *newObject = new TruecryptVerboseListMountPointDetailsObject();
                        newObject->TcContainerPath = valueString;
                        //newObject->TcMounted = true;
                        m_ListOfMountedContainers->append(newObject);

                        break;
                    }
                }
                if(breakOut)
                {
                    //mark that this one isn't on our input list and should therefore be ignored
                    getRelevantKeysBecauseWeSawTheWordVolume = false;
                }
               continue; //we're done here till next line, regardless of if it's a relevant volume
            }

            if(!getRelevantKeysBecauseWeSawTheWordVolume || m_ListOfMountedContainers->size() < 1 /*last chance error checking, allowing us to call last() safely*/)
            {

                continue; //don't go any further because we haven't seen the word volume for a relevant container (note: this strategy will get the wrong "Slot" if i ever extract it (i won't (famous last words)))
            }

            if(keyString == QString("mount directory"))
            {
                m_ListOfMountedContainers->last()->TcMountPath = valueString;
                continue; //done till next line
            }
            else if(keyString == QString("read-only"))
            {
                m_ListOfMountedContainers->last()->TcMountedAsReadOnly = (valueString.toLower() == "no" ? false : true);
                continue;
            }
        }
        if(m_ListOfMountedContainers->size() != listOfRelevantContainers->size())
        {
            if(!m_MountedAsReadOnlyFlagForDismount) //exception to the all must be mounted vs. all must be dismounted rule is when we've mounted the first one as read-only. TODOoptional: mount one of the mount points RANDOMLY so it uses round-robin distribution of read-only which-drive-to-mounts. a requirement of that optional feature is tht the dismount dismounts the right one... not longer the 'first' as currently
            {
                //we know that we're not all dismounted because we did that check way earlier.. so the only other option is that it's the size of the input listOfRelevantContainers after the scan. we do NOT deal with the partial mount case (though i guess we could, i don't prefer it (with exception to single mounts/dismounts when 'read-only' is checked TODOreq: that is a special case in dismount() slot as well as this one obviously)
                emit d("didn't find all relevant containers mounted and not all dismounted, erroring out");
                emit brokenStateDetected();
                return;
            }
        }

        bool whatToCheckAgainst = m_ListOfMountedContainers->at(0)->TcMountedAsReadOnly;
        int resultsArraySize = m_ListOfMountedContainers->size();
        for(int i = 1; i < resultsArraySize; ++i)
        {
            if(m_ListOfMountedContainers->at(i)->TcMountedAsReadOnly != whatToCheckAgainst)
            {
                emit d("read-only mismatch detected. your volumes must be all read only or all read/write. fix in truecrypt gui and re-launch");
                emit brokenStateDetected();
                return;
            }
        }

        emit d("somehow got to the end of our scanForMount function, this should ALWAYS happen :)");

        emit d("here's my results array:");
        for(int i = 0; i < resultsArraySize; ++i)
        {
            emit d("");
            emit d(QString("----detected mounted volume #") + QString::number(i) + QString("----"));
            TruecryptVerboseListMountPointDetailsObject *object = m_ListOfMountedContainers->at(i);
            emit d(object->TcContainerPath);
            emit d(object->TcMountPath);
            emit d(object->TcMountedAsReadOnly ? QString("Read-Only") : QString("Read/Write"));
            emit d("");
        }
        return;
    }
    emit d("somehow got to the end of our verbose list function. this should never happen");
    emit brokenStateDetected();
    return;
}
bool QuickDirtyAutoBackupHalperBusiness::allContainerAndPwFilePathsAreValid(QList<QPair<QString, QString> > *pathsToCheck)
{
    int pathListSize = pathsToCheck->size();
    for(int i = 0; i < pathListSize; ++i)
    {
        if(!existsAndIsNotDirNorSymLink(pathsToCheck->at(i).first))
        {
            emit d(QString("error: the following container either doesn't exist, is a dir, is a symlink, or is a bundle: ") + pathsToCheck->at(i).first);
            return false;
        }
        if(!existsAndIsNotDirNorSymLink(pathsToCheck->at(i).second))
        {
            emit d(QString("error: the following pw file either doesn't exist, is a dir, is a symlink, or is a bundle: ") + pathsToCheck->at(i).second);
            return false;
        }
    }
    return true;
}
bool QuickDirtyAutoBackupHalperBusiness::existsAndIsNotDirNorSymLink(const QString &pathToCheck)
{
    m_FileInfo.setFile(pathToCheck);
    if(!m_FileInfo.exists())
    {
        return false;
    }
    if(m_FileInfo.isDir())
    {
        return false;
    }
    //guess linux devices don't count as files. i bet they don't in windows either :-P
    /*if(!m_FileInfo.isFile())
    {
        return false;
    }*/
    if(m_FileInfo.isSymLink())
    {
        return false;
    }
    if(m_FileInfo.isBundle()) //idk wtf a bundle is but fuck apple and just to be safe ima filter it out
    {
        return false;
    }
    return true;
}
bool QuickDirtyAutoBackupHalperBusiness::probeMountStatusOfInternalListAndEmitIfEitherAllMountedOrAllDismounted()
{
    //method probes mount status of m_InternalTcContainerAndPasswordPathsList and returns 'fucked'. fucked is if any errors at all or if some containers are mounted and some are dismounted. it needs to be all or nothing for either. it emits readSettingsAndProbed() only whenever not fucked.

    if(m_InternalTcContainerAndPasswordPathsList->size() == 0)
    {
        emit readSettingsAndProbed(false, false, m_InternalTcContainerAndPasswordPathsList);
        return false;
    }

    if(!noDuplicatesExistInList(m_InternalTcContainerAndPasswordPathsList))
    {
        //if duplicates

        emit d("duplicates found in settings list");
        return true;
    }


    scanForMountedRelevantContainers(m_InternalTcContainerAndPasswordPathsList);


    int mountedContainersCount = m_ListOfMountedContainers->size();
    if(mountedContainersCount != 0)
    {
        if(mountedContainersCount != m_InternalTcContainerAndPasswordPathsList->size())
        {
            emit d("some mounted, some not. rememedy in truecrypt and re-launch");
            emit brokenStateDetected();
            return true;
        }
    }
    else
    {
        //all dismounted case
        emit readSettingsAndProbed(false, false, m_InternalTcContainerAndPasswordPathsList);
        return false;
    }
    //if we get here, that means that all relevant ones are mounted
    //it also guarantees us that at least one is mounted (we handled all dismounted case earlier)... so we can call at(0) safely
    emit readSettingsAndProbed(true, m_ListOfMountedContainers->at(0)->TcMountedAsReadOnly, m_InternalTcContainerAndPasswordPathsList);
    return false;
}
bool QuickDirtyAutoBackupHalperBusiness::noneRelevantAreAlreadyMounted(QList<QPair<QString, QString> > *listOfRelevant)
{
    scanForMountedRelevantContainers(listOfRelevant);

    if(m_ListOfMountedContainers->size() == 0)
        return true;

    return false;
}
bool QuickDirtyAutoBackupHalperBusiness::noDuplicatesExistInList(QList<QPair<QString, QString> > *list)
{
    int internalListCount = list->size();
    for(int i = 0; i < internalListCount; ++i)
    {
        for(int j = 0; j < internalListCount; ++j)
        {
            if(i == j)
                continue; //don't check ourselves, of course it will match!
            if(list->at(i).first == list->at(j).first)
            {
                return false;
            }
        }
    }
    return true;
}
bool QuickDirtyAutoBackupHalperBusiness::verifyAllAreMounted(QList<QPair<QString, QString> > *listToVerify)
{
    scanForMountedRelevantContainers(listToVerify);

    if(m_ListOfMountedContainers->size() != listToVerify->size())
    {
        emit d("verify all are mounted test failed");
        return false;
    }
    return true;
}
void QuickDirtyAutoBackupHalperBusiness::dismountEverythingRegardlessOfItBeingMineOrNot()
{
    QStringList dismountArgs(m_TypicalTruecryptArgs);
    dismountArgs << "--dismount";

    m_Process->start(m_PathToTruecryptBinary, dismountArgs);

    if(!m_Process->waitForStarted())
    {
        emit d("failed to start truecrypt binary for dismount");
        return;
    }
    if(!m_Process->waitForFinished())
    {
        emit d("truecrypt failed to finish during dismount");
        return;
    }

    int exitCode = m_Process->exitCode();
    if(exitCode != 0)
    {
        emit d("truecrypt dismount exit code was not zero");
        return;
    }

    QString dismountResultString = m_Process->readAll();

    if(dismountResultString.isNull() || dismountResultString.trimmed().isEmpty())
    {
        emit d("dismount truecrypt didn't spit anything out");
    }

    emit d(QString("Truecrypt Dis-Mount Results: ") + dismountResultString);

    if(dismountResultString.contains(QString("Error:"), Qt::CaseInsensitive))
    {
        emit d("got an error while dismounting");
        return;
    }
    else if(dismountResultString.contains(QString("has been dismounted."), Qt::CaseInsensitive))
    {
        emit d("detected that at least one was dismounted during dismount of everything");
    }
    //TODOoptional: catch the 'is busy and cannot be dismounted right now' message and send it to the GUI, perhaps stopping the shutdown process. right now we'll depend on the user pressing the CANCEL button after reading the dismountResultString
}
void QuickDirtyAutoBackupHalperBusiness::start3secondShutdownTimerDispatchingUpdatesAtEachSecondInterval()
{
    if(!m_ShutdownTimer->isActive())
    {
        m_SecondsLeftUntilShutdown = DEFAULT_SHUTDOWN_SECONDS_COUNTDOWN_AMOUNT;
        //m_ShutdownTimer->start(m_SecondsLeftUntilShutdown * 1000);
        m_ShutdownTimer->start(1000); //we want one second update-ing, not just one timeout at the last second
    }
    emit dismounted(); //so the gui is up to date if they happen to hit cancel shutdown button
}
void QuickDirtyAutoBackupHalperBusiness::actuallyCommitToListOfMountedContainers(const QString &commitMsg, const QString &workingDirString, const QString &subDirOnMountPointToBareGitRepo, const QString &dirStructureFileNameString, QStringList *filenameIgnoreList, QStringList *filenameEndsWithIgnoreList, QStringList *dirnameIgnoreList, QStringList *dirnameEndsWithIgnoreList, bool pushEvenWhenNothingToCommit)
{
    QString tempHackyEnsureEndInSlashString;
    tempHackyEnsureEndInSlashString.append(workingDirString);
    if(!tempHackyEnsureEndInSlashString.endsWith("/"))
    {
        tempHackyEnsureEndInSlashString.append("/");
    }

    //git status. verify this the following. after verify, emit d("nothing to commit"); perhaps we still need to do git push though... we don't know! running the commands (any of them) and being wrong is cheap anyways.. but the tree file might re-gen and trigger a useless commit, that's the thing. we could catch it and skip the commit phase and just do git potentially worthless git pushes instead. no risk of "useless commit", and also no data loss because we VEFIFY the following from git status:
    /*
# On branch master
nothing to commit (working directory clean)
    */
    QStringList gitStatusArgs;
    gitStatusArgs << "status";
    m_Process->setWorkingDirectory(tempHackyEnsureEndInSlashString);
    m_Process->start(m_PathToGitBinary, gitStatusArgs, QIODevice::ReadOnly | QIODevice::Text);
    if(!m_Process->waitForStarted(999999999))
    {
        emit d("git status failed to start");
        emit brokenStateDetected();
        return;
    }
    if(!m_Process->waitForFinished(999999999))
    {
        emit d("git status failed to finish");
        emit brokenStateDetected();
        return;
    }
    int exitCode = m_Process->exitCode();
    if(exitCode != 0)
    {
        emit d(QString("error: git status gave us exit code: ") + QString::number(exitCode));
        emit brokenStateDetected();
        return;
    }
    QTextStream gitStatusTextStream(m_Process);
    QString currentGitStatusLine = gitStatusTextStream.readLine();
    bool nothingToCommit = false;
    if(currentGitStatusLine.startsWith("# On branch ", Qt::CaseInsensitive))
    {
        currentGitStatusLine = gitStatusTextStream.readLine();
        if(currentGitStatusLine.startsWith("nothing to commit (working directory clean)"))
        {
            nothingToCommit = true;
            emit d("nothing to commit. skipping commit, but still pushing");
        }
    }

    if(!nothingToCommit)
    {
        //generate tree

        //now i KNOW it ends with a slash, so i just append the filename of the dir structure file
        tempHackyEnsureEndInSlashString.append(dirStructureFileNameString); //concat the working dir path to the dir structure filename, so we don't write the dir structure file to this executable's working dir lmao
        QFile dirStructureFile(tempHackyEnsureEndInSlashString);
        if(!dirStructureFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            emit d("failed to open dir structure file for writing");
            emit brokenStateDetected();
            return;
        }

        m_FileIgnoreListForTreeOnlyWithTheDirStructureFileIncluded->clear();
        m_FileIgnoreListForTreeOnlyWithTheDirStructureFileIncluded->append(*filenameIgnoreList); //cat on the passed in list
        m_FileIgnoreListForTreeOnlyWithTheDirStructureFileIncluded->append(dirStructureFileNameString); //tell tree to ignore the file it generates. TODOoptional: perhaps a delete after the commit would be handy so we don't have to deal with renaming problems? if i rename the dir structure file, the old one will continue to exist (and will NOT be ignored, so it will be added to the renamed dir structure file. big whoop)

        m_EasyTree->generateTreeText(workingDirString, &dirStructureFile, EasyTree::DontCalculateMd5Sums, dirnameIgnoreList, m_FileIgnoreListForTreeOnlyWithTheDirStructureFileIncluded, filenameEndsWithIgnoreList, dirnameEndsWithIgnoreList, EasyTree::SimplifiedLastModifiedTimestamps);

        if(!dirStructureFile.flush())
        {
            emit d("flushing the dir structure failed");
            emit brokenStateDetected();
            return;
        }
        dirStructureFile.close();

        emit d("wrote dir structure file");

        //git add
        QStringList gitAddArgs;
        gitAddArgs << "add" << ".";
        m_Process->start(m_PathToGitBinary, gitAddArgs, QIODevice::ReadOnly | QIODevice::Text);
        if(!m_Process->waitForStarted(999999999))
        {
            emit d("git add failed to start");
            emit brokenStateDetected();
            return;
        }
        if(!m_Process->waitForFinished(999999999))
        {
            emit d("git add failed to finish");
            emit brokenStateDetected();
            return;
        }
        exitCode = m_Process->exitCode();
        if(exitCode != 0)
        {
            emit d(QString("error: git add gave us exit code: ") + QString::number(exitCode));
            emit brokenStateDetected();
            return;
        }
        QString gitAddResults = m_Process->readAll(); //should be empty
        if(gitAddResults.contains("error", Qt::CaseInsensitive))
        {
            emit d("git add returned an error");
            emit brokenStateDetected();
            return;
        }
        emit d(QString("git add results (should be empty): ") + gitAddResults);

        //git commit using commit message
        QStringList gitCommitArgs;
        gitCommitArgs << "commit" << "-am" << commitMsg;
        m_Process->start(m_PathToGitBinary, gitCommitArgs, QIODevice::ReadOnly | QIODevice::Text);
        if(!m_Process->waitForStarted(999999999))
        {
            emit d("git commit failed to start");
            emit brokenStateDetected();
            return;
        }
        if(!m_Process->waitForFinished(999999999))
        {
            emit d("git commit failed to finish");
            emit brokenStateDetected();
            return;
        }
        exitCode = m_Process->exitCode();
        if(exitCode != 0)
        {
            emit d(QString("error: git commit gave us exit code: ") + QString::number(exitCode));
            emit brokenStateDetected();
            return;
        }
        QString gitCommitResults = m_Process->readAll(); //should be empty
#if 0
        if(gitCommitResults.contains("error", Qt::CaseInsensitive)) //TOD ONreq: a filename called "error" would show up here and give me a false positive rofl
        {
            emit d("git commit returned an error");
            emit brokenStateDetected();
            return;
        }
#endif
        emit d(QString("git commit results: ") + gitCommitResults);
    }

    //push
    if(!nothingToCommit || pushEvenWhenNothingToCommit)
    {
        //sanity is already checked here, now we rely on m_ListOfMountedContainers because it's already been checked by caller
        int mountedContainersCount = m_ListOfMountedContainers->size();
        QString appendToMountPath("/");
        if(!subDirOnMountPointToBareGitRepo.trimmed().isEmpty())
        {
            appendToMountPath.append(subDirOnMountPointToBareGitRepo);
            appendToMountPath.append("/"); //trailing slash needed when pushing methinks. i know i've seen it work at the very least
        }
        for(int i = 0; i < mountedContainersCount; ++i)
        {
            //TODOoptional: iterate through branches in working directory repo, then push those too

            //git push into each mounted container mount point + that sub-dir (need to pass it in with the commit slot)
            QStringList gitPushArgs;
            gitPushArgs << "push" << (m_ListOfMountedContainers->at(i)->TcMountPath + appendToMountPath) << "master"; //TODOoptional: support branches other than master, perhaps iterating the working dir's branches or letting the user specify the branches (or even letting them CHOOSE after I iterate the working dir...)
            m_Process->start(m_PathToGitBinary, gitPushArgs, QIODevice::ReadOnly | QIODevice::Text);
            if(!m_Process->waitForStarted(999999999))
            {
                emit d("git push failed to start");
                emit brokenStateDetected();
                return;
            }
            if(!m_Process->waitForFinished(999999999))
            {
                emit d("git push failed to finish");
                emit brokenStateDetected();
                return;
            }
            exitCode = m_Process->exitCode();
            if(exitCode != 0)
            {
                emit d(QString("error: git push gave us exit code: ") + QString::number(exitCode));
                emit brokenStateDetected();
                return;
            }
            QString gitPushResults = m_Process->readAll(); //should be empty
    #if 0
            if(gitPushResults.contains("error", Qt::CaseInsensitive)) //TOD ONEreq: branch named "error" would trigger false positive
            {
                emit d("git push returned an error");
                emit brokenStateDetected();
                return;
            }
    #endif
            emit d(QString("git push results: ") + gitPushResults);

            //TODOoptional: a progress bar that updates itself with the git push command lol, fuck that
        }

        emit d("commit (and optional push) appears to have completed because we did not error out");

        //verify
        //do verification of the working dir's git log against our commit message (and PERHAPS the time, but maybe only extracting it for the next step-)
        //do verification of the push'd locations by comparing the git commit message and/or the timestamps to the working dir's
        //i guess both time and commit log verification is the safest solution
    }
    emit committed();
}
void QuickDirtyAutoBackupHalperBusiness::actualShutdown()
{
    //QStringList shutdownArgs;
    //shutdownArgs << "-h" << "now";

    QProcess::startDetached("/usr/bin/sudo /sbin/shutdown -h now"); //just makes sense to start a shutdown as detached if you ask me! sucks we won't be reading it's debug output though... maybe i can/should use m_Process just for that TODOmb

    //TODOoptional: could start stopping ourself right here... or just wait till we get the SIGTERM from the OS... it doesn't matter i don't think.
    //^^^i guess the absofuckinglute safest way is to stop the thread / backend and free up all the memory and then call startDetached(shutdown) just before a.exec returns... so there is never racing occuring. or even thoughts of racing...
}
void QuickDirtyAutoBackupHalperBusiness::start()
{
    if(!m_Process)
    {
        m_ShutdownTimer = new QTimer(this);
        connect(m_ShutdownTimer, SIGNAL(timeout()), this, SLOT(handleShutdownTimerTimedOut()));

        m_Process = new QProcess(this);
        m_Process->setProcessChannelMode(QProcess::MergedChannels);

        m_InternalTcContainerAndPasswordPathsList = new QList<QPair<QString,QString> >();
        m_ListOfMountedContainers = new QList<TruecryptVerboseListMountPointDetailsObject*>();
        m_FileIgnoreListForTreeOnlyWithTheDirStructureFileIncluded = new QStringList();

        m_EasyTree = new EasyTree();
    }
    if(!QFile::exists(m_PathToTruecryptBinary))
    {
        emit d("couldn't find truecrypt");
        emit brokenStateDetected();
        return;
    }
    if(!QFile::exists(m_PathToGitBinary))
    {
        emit d("couldn't find git");
        emit brokenStateDetected();
        return;
    }
    readSettingsAndProbeMountStatus();
}
void QuickDirtyAutoBackupHalperBusiness::stop()
{
    if(m_Process)
    {
        delete m_ShutdownTimer;
        delete m_Process;
        m_Process = 0;

        delete m_FileIgnoreListForTreeOnlyWithTheDirStructureFileIncluded;
        delete m_ListOfMountedContainers;
        if(m_StillUsingOurOwnAllocatedInternalList)
        {
            //else our internal list points to one allocated by GUI passed in via mount(), so don't delete it
            delete m_InternalTcContainerAndPasswordPathsList;
        }

        delete m_EasyTree;
    }
}
void QuickDirtyAutoBackupHalperBusiness::pushToGitIgnore(const QString &workingDir, QStringList *filenamesIgnoreList, QStringList *filenamesEndsWithIgnoreList, QStringList *dirnamesIgnoreList, QStringList *dirnamesEndsWithIgnoreList)
{
    QString workingDirString = workingDir;
    if(!workingDirString.endsWith("/", Qt::CaseSensitive))
    {
        workingDirString.append("/");
    }
    QString gitIgnoreFilePathString(workingDirString + QString(".gitignore"));
    QString gitIgnoreContents;
    if(!QFile::exists(gitIgnoreFilePathString))
    {
        if(filenamesIgnoreList->size() == 0 && filenamesEndsWithIgnoreList->size() == 0 && dirnamesIgnoreList->size() == 0 && dirnamesEndsWithIgnoreList->size() == 0)
        {
            //if the file doesn't exist and our filter lists are empty, do not proceed. do not create a .gitignore file for nothing
            return;
        }
    }
    QFile gitIgnoreFile(gitIgnoreFilePathString);
    if(!gitIgnoreFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        emit d("error opening git ignore file for reading");
        emit brokenStateDetected();
        return;
    }

    QTextStream readStream(&gitIgnoreFile);
    gitIgnoreContents = readStream.readAll();
    gitIgnoreFile.close();



    QString specialDetectingTextBeginInIgnoreFile("#%%QuickDirtyAutoBackupHalperDefines_Begin%%");
    QString specialDetectingTextEndInIgnoreFile("#%%QuickDirtyAutoBackupHalperDefines_End%%");

    if(gitIgnoreContents.contains(specialDetectingTextBeginInIgnoreFile) && gitIgnoreContents.contains(specialDetectingTextEndInIgnoreFile))
    {
        //strip them out of the string
        int zeroIndexAtFrontOfBegin = gitIgnoreContents.indexOf(specialDetectingTextBeginInIgnoreFile);

        int indexAfterEndOfEnd = gitIgnoreContents.indexOf(specialDetectingTextEndInIgnoreFile) + specialDetectingTextEndInIgnoreFile.length();
        gitIgnoreContents.remove(zeroIndexAtFrontOfBegin, (indexAfterEndOfEnd-zeroIndexAtFrontOfBegin));
    }

    QString newLine("\n");
    QString currentFilter;

    if(filenamesIgnoreList->size() == 0 && filenamesEndsWithIgnoreList->size() == 0 && dirnamesIgnoreList->size() == 0 && dirnamesEndsWithIgnoreList->size() == 0)
    {
        //do nothing. we already stripped it out, but gitIgnoreContents currently holds the user's OTHER git ignore settings, so we still want to proceed and re-write them to the file. we may or may not have stripped out previous entries
    }
    else
    {
        //add our defines, including begin/end, to the string

        //begin
        gitIgnoreContents.append(specialDetectingTextBeginInIgnoreFile + newLine);

        QListIterator<QString> it(*filenamesIgnoreList);
        while(it.hasNext())
        {
            currentFilter = it.next().trimmed();
            if(currentFilter.isEmpty())
            {
                continue;
            }
            gitIgnoreContents.append(currentFilter + newLine);
        }
        QString starString("*");
        it = *filenamesEndsWithIgnoreList;
        while(it.hasNext())
        {
            currentFilter = it.next().trimmed();
            if(currentFilter.isEmpty())
            {
                continue;
            }
            gitIgnoreContents.append(starString + currentFilter + newLine);
        }
        QString slashString("/");
        it = *dirnamesIgnoreList;
        while(it.hasNext())
        {
            currentFilter = it.next().trimmed();
            if(currentFilter.isEmpty() || currentFilter == ".git" /*hack because git already ignores it's own git folder, but this list is also used by EasyTree, which does want to ignore .git*/)
            {
                continue;
            }
            gitIgnoreContents.append(currentFilter + slashString + newLine);
        }
        it = *dirnamesEndsWithIgnoreList;
        while(it.hasNext())
        {
            currentFilter = it.next().trimmed();
            if(currentFilter.isEmpty())
            {
                continue;
            }
            gitIgnoreContents.append(starString + currentFilter + slashString + newLine);
        }

        gitIgnoreContents.append(specialDetectingTextEndInIgnoreFile + newLine);
    }

    QStringList tempSplitByLines = gitIgnoreContents.split("\n", QString::SkipEmptyParts);
    QString gitIgnoreContentsWithEmptyLinesRemoved;
    QListIterator<QString> it(tempSplitByLines);
    while(it.hasNext())
    {
        currentFilter = it.next().trimmed();
        if(currentFilter.isNull() || currentFilter.isEmpty())
        {
            continue; //skip empty lines, is they even still exist. i am squashing them pretty much. SkipEmptyParts, trimmed(), and this isEmpty... ONE OF THEM will get it
        }
        gitIgnoreContentsWithEmptyLinesRemoved.append(currentFilter + newLine);
    }


    if(!gitIgnoreFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        emit d("error opening git ignore file for writing");
        emit brokenStateDetected();
        return;
    }
    gitIgnoreFile.resize(0);
    QTextStream textStream(&gitIgnoreFile);
    textStream << gitIgnoreContentsWithEmptyLinesRemoved;
    //gitIgnoreFile.write(gitIgnoreContents);
    textStream.flush();
    if(!gitIgnoreFile.flush())
    {
        emit d("error flushing git ignore file");
        emit brokenStateDetected();
        return;
    }
    gitIgnoreFile.close();
    emit d("successfully pushed to .gitignore");
}
void QuickDirtyAutoBackupHalperBusiness::persist(QList<QPair<QString, QString> > *tcContainerAndPasswordPathsToPersist)
{
    if(allContainerAndPwFilePathsAreValid(tcContainerAndPasswordPathsToPersist))
    {
        QSettings settings;
        settings.beginWriteArray(QString(TC_CONTAINERS_AND_PW_FILES_ARRAY_STRING));
        settings.clear(); //clear existing settings, we will re-write them
        int pathListSize = tcContainerAndPasswordPathsToPersist->size();
        for(int i = 0; i < pathListSize; ++i)
        {
            settings.setArrayIndex(i);
            settings.setValue(QString(TC_CONTAINERS_PATH_STRING_KEY), tcContainerAndPasswordPathsToPersist->at(i).first);
            settings.setValue(QString(TC_PASSWORD_FILE_PATH_STRING_KEY), tcContainerAndPasswordPathsToPersist->at(i).second);
        }
        settings.endArray();
        emit d(QString("persisted successfully to: ") + settings.fileName());
    }
    else
    {
        emit d("not persisting any because of existence of at least one invalid path. see above errors");
    }
}
void QuickDirtyAutoBackupHalperBusiness::mount(QList<QPair<QString, QString> > *tcContainerAndPasswordPathsToMount, bool mountAsReadOnly)
{
    if(allContainerAndPwFilePathsAreValid(tcContainerAndPasswordPathsToMount))
    {
        if(noDuplicatesExistInList(tcContainerAndPasswordPathsToMount))
        {
            if(noneRelevantAreAlreadyMounted(tcContainerAndPasswordPathsToMount)) //TODOreq: noneRelevantAreAlreadyDismounted -- the opposite -- should also emit brokenStateDetected(); -- PERHAPS with exception to dismounting just before shutdown (bool ignoreDismountErrorOfAlreadyDismounted comes to mind)... we might really need to shutdown fast and not care about the error. but it should still dismount the rest :)
            {
                int containersToMountCount = tcContainerAndPasswordPathsToMount->size();
                QFile passwordFile;
                QString passwordArg;
                m_MountedAsReadOnlyFlagForDismount = mountAsReadOnly;
                if(mountAsReadOnly && (containersToMountCount > 1))
                {
                    //mount first only impl/hack, just make the for loop end at one lmfao
                    containersToMountCount = 1;
                }
                for(int i = 0; i < containersToMountCount; ++i)
                {
                    passwordFile.setFileName(tcContainerAndPasswordPathsToMount->at(i).second);
                    passwordFile.open(QIODevice::ReadOnly | QIODevice::Text);
                    QTextStream stream(&passwordFile);

                    passwordArg.clear();
                    passwordArg.append("--password=");
                    QString password = stream.readLine();
                    passwordFile.close();
                    if(password.isNull() || password.trimmed().isEmpty())
                    {
                        emit d(QString("password in file: ") + tcContainerAndPasswordPathsToMount->at(i).second + QString(" is empty"));
                        //TODOreq: dismount all the ones mounted in previous iterations of this for loop. i should use a function here and down below somewhere where i do the same operation. it is like a roll back of sorts
                        return;
                    }
                    passwordArg.append(password);

                    QStringList mountArgs(m_TypicalTruecryptArgs);
                    //i wonder if this is a security vuln... passing input onto command line like this..
                    mountArgs << passwordArg << tcContainerAndPasswordPathsToMount->at(i).first;
                    if(mountAsReadOnly)
                    {
                        mountArgs << "-m" << "ro";
                    }

                    m_Process->start(m_PathToTruecryptBinary, mountArgs);

                    if(!m_Process->waitForStarted())
                    {
                        emit d("failed to start truecrypt binary for mount");
                        return;
                    }
                    if(!m_Process->waitForFinished())
                    {
                        emit d("truecrypt failed to finish during mount");
                        return;
                    }

                    int exitCode = m_Process->exitCode();
                    if(exitCode != 0)
                    {
                        emit d("truecrypt mount exit code was not zero, erroring out");
                        //TODOreq: dismount prior
                        emit brokenStateDetected();
                        return;
                    }

                    QString mountResultString = m_Process->readAll();

                    if(mountResultString.isNull() || mountResultString.trimmed().isEmpty())
                    {
                        emit d("mount truecrypt didn't spit anything out");
                    }

                    emit d(QString("Truecrypt Mount Results: ") + mountResultString);

                    if(mountResultString.contains(QString("Error:"), Qt::CaseInsensitive))
                    {
                        //TODOreq: maybe since this one error'd we want to now dismount all the ones mounted prior (but ignore non-relevant containers!). perhaps i could build a list of previously mounted containers and pass it to dismount()

                        emit d("truecrypt mount returned an error");
                        emit brokenStateDetected();
                        return;
                    }
                    else if(mountResultString.contains(QString("has been mounted."), Qt::CaseInsensitive))
                    {
                        continue; //whitelist programming ftw
                    }
                    emit d("got weird results when trying to mount");
                    emit brokenStateDetected();
                    return;
                }
                emit d("all appear to have been mounted because we didn't see any errors");

                //we now override our internal list (which up till now only contained whatever may be in the QSettings)
                //we do this because dismount needs to both work when QSettings read in and all the settings specified ones are mounted
                //and when we mount manually, aka right here
                if(m_StillUsingOurOwnAllocatedInternalList)
                {
                    //hack so we only delete the m_InternalTcContainerAndPasswordPathsList when WE have allocated it. we remember if it was overriden by a mount()
                    m_StillUsingOurOwnAllocatedInternalList = false;
                    delete m_InternalTcContainerAndPasswordPathsList;
                }
                m_InternalTcContainerAndPasswordPathsList = tcContainerAndPasswordPathsToMount;
                //since we're mounted after emitting mounted() on the next line, the tc container rows become read-only so we don't have to worry about it changing
                emit mounted();
            }
            else
            {
                //getting here: launch our app with all dismounted. in truecrypt, mount one relevant (we ignore irrelevant). then try mounting in our app
                //broken state, error the fuck out :-)
                emit d("uhh we detected that one that we were about to mount is already mounted (and it wasn't at app launch). don't do that shit");
                emit brokenStateDetected();
            }
        }
        else
        {
            emit d("at least two of your containers are the same file. none can be");
        }
    }
    else
    {
        emit d("not mounting any because of existence of at least one invalid path. see above errors");
    }
}
void QuickDirtyAutoBackupHalperBusiness::dismount()
{
    if(verifyAllAreMounted(m_InternalTcContainerAndPasswordPathsList) || m_MountedAsReadOnlyFlagForDismount)
    {
        int mountedCount = m_InternalTcContainerAndPasswordPathsList->size();
        if(m_MountedAsReadOnlyFlagForDismount && (mountedCount > 1))
        {
            mountedCount = 1;
        }
        m_MountedAsReadOnlyFlagForDismount = false; //put it back in it's default state, though this really doesn't matter as another call to mount() will set it as appropriately
        for(int i = 0; i < mountedCount; ++i)
        {
            QStringList dismountArgs(m_TypicalTruecryptArgs);
            dismountArgs << "--dismount" << m_InternalTcContainerAndPasswordPathsList->at(i).first;

            m_Process->start(m_PathToTruecryptBinary, dismountArgs);

            if(!m_Process->waitForStarted())
            {
                emit d("failed to start truecrypt binary for dismount");
                return;
            }
            if(!m_Process->waitForFinished())
            {
                emit d("truecrypt failed to finish during dismount");
                return;
            }

            int exitCode = m_Process->exitCode();
            if(exitCode != 0)
            {
                emit d("truecrypt dismount exit code was not zero, erroring out");
                emit brokenStateDetected();
                return;
            }

            QString dismountResultString = m_Process->readAll();

            if(dismountResultString.isNull() || dismountResultString.trimmed().isEmpty())
            {
                emit d("dismount truecrypt didn't spit anything out");
            }

            emit d(QString("Truecrypt Dis-Mount Results: ") + dismountResultString);

            if(dismountResultString.contains(QString("Error:"), Qt::CaseInsensitive))
            {
                emit d("got an error while dismounting");
                emit brokenStateDetected();
                return;
            }
            else if(dismountResultString.contains(QString("has been dismounted."), Qt::CaseInsensitive))
            {
                continue; //whitelist programming ftw
            }
            emit d("got weird results when trying to dismount");
            emit brokenStateDetected();
            return;
        }
        emit d("all appear to have been dismounted because we didn't see any errors");
        emit dismounted();
    }
    else
    {
        emit d("you dismounted some while the application was running. we won't even attempt to dismount any others. don't do that again");
        emit brokenStateDetected();
        return;
    }
}
void QuickDirtyAutoBackupHalperBusiness::commit(const QString &commitMsg, QList<QPair<QString, QString> > *tcContainerAndPasswordPathsFromGui, const QString &workingDirString, const QString &subDirOnMountPointToBareGitRepo, const QString &dirStructureFileNameString, QStringList *filenameIgnoreList, QStringList *filenameEndsWithIgnoreList, QStringList *dirnameIgnoreList, QStringList *dirnameEndsWithIgnoreList, bool pushEvenWhenNothingToCommit)
{
    //the entire commit should happen within this method. we should use QProcess's blocking functions
    if(!verifyAllAreMounted(tcContainerAndPasswordPathsFromGui))
    {
        if(m_ListOfMountedContainers->size() != 0)
        {
            emit d("some mounted some not. remedy in truecrypt gui and re-launch");
            emit brokenStateDetected();
            return;
        }

        //we now know that NONE of them are mounted, so we can safely doooeeeet

        mount(tcContainerAndPasswordPathsFromGui, false);
    }

    if(verifyAllAreMounted(tcContainerAndPasswordPathsFromGui)) //check again, to make sure the mount actually proceeded
    {
        if(m_ListOfMountedContainers->size() != tcContainerAndPasswordPathsFromGui->size())
        {
            emit d("weird results when two container sizes that should match don't");
            emit brokenStateDetected();
            return;
        }
        if(m_ListOfMountedContainers->size() > 0) //can commit to zero containers, noob
        {
            if(m_ListOfMountedContainers->at(0)->TcMountedAsReadOnly)
            {
                //mismatch case already handled, but we still need to make sure the end result is ok
                emit d("can't commit when read-only. we should never get here ideally");
                emit brokenStateDetected();
                return;
            }
        }
        m_FileInfo.setFile(workingDirString);
        if(!m_FileInfo.exists())
        {
            emit d("working dir does not exist fool");
            //fixable
            return;
        }
        if(!m_FileInfo.isDir())
        {
            emit d("working dir not a dir");
            //fixable
            return;
        }

        //verify existence of the bare repo on each of the mount points
        int mountedCountainersCount = m_ListOfMountedContainers->size();
        for(int i = 0; i < mountedCountainersCount; ++i)
        {
            m_FileInfo.setFile(m_ListOfMountedContainers->at(i)->TcMountPath + QString("/") + subDirOnMountPointToBareGitRepo);
            if(!m_FileInfo.exists())
            {
                emit d(QString("the sub-dir: ") + subDirOnMountPointToBareGitRepo + QString(" on: ") + m_ListOfMountedContainers->at(i)->TcMountPath + QString(" does not exist and should")); //TODOoptional: i've discussed a checkmark that could auto-create the dir at this point...
                emit brokenStateDetected();
                return;
            }
            if(!m_FileInfo.isDir())
            {
                emit d(QString("the sub-dir: ") + subDirOnMountPointToBareGitRepo + QString(" on: ") + m_ListOfMountedContainers->at(i)->TcMountPath + QString(" isn't actually a dir"));
                emit brokenStateDetected();
                return;
            }
            //TODOoptinal: verify the existence of files that indicate it's a bare repo dir. HOOKS file etc. i can just get the entryList or do a m_FileInfo.setFile ... and then .exists()
        }

        actuallyCommitToListOfMountedContainers(commitMsg, workingDirString, subDirOnMountPointToBareGitRepo, dirStructureFileNameString, filenameIgnoreList, filenameEndsWithIgnoreList, dirnameIgnoreList, dirnameEndsWithIgnoreList, pushEvenWhenNothingToCommit);
        //TODOreq/___NOTE____: no code should come after the above line. it sometimes does 'return' in error cases after emitting brokenStateDetected. a better solution would be to check a bool but fuck it for now~
    }
    else
    {
        emit d("mount must have failed, so not committing. see above errors");
        emit brokenStateDetected();
        return;
    }
}
void QuickDirtyAutoBackupHalperBusiness::shutdown()
{
    /*if(commitIfNecessary && thereIsSomethingToCommit())
    {
        commit(); //does push based on idfk because we usually pass in a checkbox value. guess i could pass it in again... gah
    }*/ //--FIX'D ;-) via hack in gui

    dismountEverythingRegardlessOfItBeingMineOrNot();

    m_ShuttingDown = true;
    start3secondShutdownTimerDispatchingUpdatesAtEachSecondInterval();

    //shutTheFuckDown();
}
void QuickDirtyAutoBackupHalperBusiness::cancelShutdown()
{
    m_ShuttingDown = false;
    m_ShutdownTimer->stop();
    emit d("cancelling shutdown");
    //setAboolShutdownToFalseAndThenStopTheMotherfuckingTimerAndThenSayYouStoppedItToGuiBitchAssSlut(); //the bool is double checked by the very last timer time out to really make sure we don't want to shutdown. random race condition: you click shutdown and your shutdown event just happens to jump in front of the already-dispatched-but-yet-to-be-processed-timer-timeout.. or perhaps you click cancel JUST BEFORE it. and that's assuming that the timer timeout doesn't already validate that it's timer is still isRunning. it might but i haven't the slightest. race condition i'll probably NEVER EVER SEE!
}
void QuickDirtyAutoBackupHalperBusiness::handleShutdownTimerTimedOut()
{
    emit d(QString("Shutting Down In...") + QString::number(m_SecondsLeftUntilShutdown));
    if(m_SecondsLeftUntilShutdown < 1)
    {
        m_ShutdownTimer->stop();
        if(m_ShuttingDown)
        {
            actualShutdown();
        }
    }
    else
    {
        --m_SecondsLeftUntilShutdown;
    }
}
