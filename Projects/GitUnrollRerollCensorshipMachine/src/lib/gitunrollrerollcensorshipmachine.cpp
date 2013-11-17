#include "gitunrollrerollcensorshipmachine.h"

#define CUSTOM_GIT_UNROLL_REROLL_HACKS_FOR_D3FAULT 1

#ifdef CUSTOM_GIT_UNROLL_REROLL_HACKS_FOR_D3FAULT
#include <QProcess>
#include "filemodificationdatechanger.h"
#include "lastmodifieddateheirarchymolester.h"
#include "easytree.h"
#endif

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
        git --work-tree=/path/to/working/dir/ --git-dir=/path/to/dotGitFolderLooksBareButIsnt checkout --force <commit-id>

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
    if(!m_GitHelper.gitLogReturningCommitIdsAuthorDateAndCommitMessage(allCommitSha1sFromGitLogCommand, absoluteCensoredSourceDirsAssociatedDetachedGitFolder))
    {
        emit d("failed to run git log in order to iterate the history of the censored repo");
        return;
    }

#ifdef CUSTOM_GIT_UNROLL_REROLL_HACKS_FOR_D3FAULT
    QString gitIgnoreFileGeneratedAndHeldInPlaceFilePath = absoluteActualWorkingDir + ".gitignore";
    QFile gitIgnoreFileGeneratedAndHeldInPlace(gitIgnoreFileGeneratedAndHeldInPlaceFilePath);
    QStringList gitIgnoreLines;
    gitIgnoreLines << "*.[ao]" << "*.pro.user" << "*~" << "*__Qt_SDK__Debug/" << "*__Qt_SDK__Release/" << "*_in_PATH__System__Debug/" << "*_in_PATH__System__Release/";
    QTextStream gitIgnoreTextStream(&gitIgnoreFileGeneratedAndHeldInPlace);
    FileModificationDateChanger fileModificationChanger;
    bool firstTimeCreatingGitIgnore = true;
    QDateTime gitIgnoreTimestamp;

    //we escape the filename sent to find in case it has a space, and since we're using the string execute method
    QString findChmodCommand = "/usr/bin/find \"" + absoluteActualWorkingDir + "\" + ( -type d -exec /bin/chmod 755 {} ; ) -o ( -type f -exec /bin/chmod 644 {} ; )"; //took me longer than expected to figure out why `chmod -R 644 dir/` wouldn't work xD (stupid coders)

    LastModifiedDateHeirarchyMolester heirarchyMolester;
    connect(&heirarchyMolester, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    QList<QString> dirNamesToIgnore;
    dirNamesToIgnore.append(".git");
    QList<QString> fileNamesToIgnore;
    //none
    QList<QString> fileNamesEndWithIgnoreList;
    fileNamesEndWithIgnoreList.append(".pro.user");
    fileNamesEndWithIgnoreList.append("~");
    fileNamesEndWithIgnoreList.append(".a");
    fileNamesEndWithIgnoreList.append(".o");
    QList<QString> dirNamesEndsWithIgnoreList;
    dirNamesEndsWithIgnoreList.append("__Qt_SDK__Debug");
    dirNamesEndsWithIgnoreList.append("__Qt_SDK__Release");
    dirNamesEndsWithIgnoreList.append("_in_PATH__System__Debug");
    dirNamesEndsWithIgnoreList.append("_in_PATH__System__Release");
#endif

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

        //TODOreq: checkout doesn't work how I thought it did, it leaves changes on tracked files (such as deletions). I wonder if chmod'ing will make files "changed" and therefore miss "future revisions" by "not checking out [new revisions] because it thinks i changed them and don't want to" <-- solution would be to either not modify the checkout and modify a copy of the checkout, or clone over and over and over (those two are the same solution really, just different means). I actually think yes I would miss "revisions", but idfk. (GOOD DAMN CATCH SHIIIIIIIT computers are scary). A way to verify that we "got all changes" is to run easyTreeHash on the before/after results and compare (obviously censored files will show up :-P)
        //I think setting '--force' when doing git checkout solves the above...
#ifdef CUSTOM_GIT_UNROLL_REROLL_HACKS_FOR_D3FAULT

        //TODOreq: make sure no files I modify/create have parent directories that are relevant to the output timestamp file. I was thinking the output timestamp file itself would qualify, but we don't record the timestamp of ".", so as of right now I can't think of any cases. Also worth noting that deleting a file modifies the directory last modified date...

        //CHMOD Everything because I don't give a fuck and don't want git to record it...

        int findChmodReturnCode = QProcess::execute(findChmodCommand);
        if(findChmodCommand != 0)
        {
            emit d("find chmod command didn't return 0: " + QString::number(findChmodReturnCode));
            return;
        }

        //MOLEST STAGE 1/3 -- FILL TABLE

        //TODOreq: we need to verify that all the files in the heirarchy were molested (maybe not all are in the list), otherwise it'll have a timestamp at the time of execution. If it wasn't molested, give it a sane timestamp (checkout-1, for example)

        if(QFile::exists(absoluteActualWorkingDir + "dirstructure.txt")) //early days
        {
            if(QFile::exists(absoluteActualWorkingDir + ".quickDirtyAutoBackupHalperDirStructure"))
            {
                emit d("fucked state .quickDirtyAutoBackupHalperDirStructure exists and shouldn't");
                return;
            }

            if(!heirarchyMolester.loadFromXml(absoluteActualWorkingDir, absoluteActualWorkingDir + "dirstructure.txt"))
            {
                emit d("failed to load dirstructure.txt for heirarchy molester");
                return;
            }
        }
        else if(QFile::exists(absoluteActualWorkingDir + ".dirstructure.txt.old.from.tree.command") && !QFile::exists(absoluteActualWorkingDir + ".quickDirtyAutoBackupHalperDirStructure")) //intermediate where some timestamps are lost
        {
            emit d("boner");
            //TODOreq -- I've got everything for before/after coded and ready for testing, but I can't for the life of me figure out what to do here xD. Probably something simple... but guh. I know this is a vital part for letting "after" use "before"'s timestamps
            //maybe just touch EVERY timestamp to the same timestamp so my 'performHackyOccurance' shit takes over and uses the "old" table... and then "new"/renamed files will get picked up in 2/3 MOLEST and.... aww shit this won't work because performHackyOccurance[...] needs a 'current' table to be populated, but I have nothing to populate it with :-/.
            //Maybe I just do a manual iteration/population (similar to 2/3) and give them all the same timestamp. Since I do that now/before performHackyOccurance, it means we'll have the effect of pulling from the old table. I _THINK_ we can even use the same method as 2/3, but it would mean that we have a redundant call to 2/3 just once (DGAF)
#if 0
            QDateTime timeStampForSpecialInBetweenFiles = QDateTime::fromString(commitIdTimestampAndMessage->commitDate, Qt::ISODate);
            timeStampForSpecialInBetweenFiles = timeStampForSpecialInBetweenFiles.addSecs(-1);

            //clear out the 'current' (which is the same as 'old') table, but keep the 'old' one for when performHackyOccuranceRateMerging is called

            if(!heirarchyMolester.loadAnyMissedFilesByRecursivelyScanningDirectoriesAndGiveThemThisTimestamp(absoluteActualWorkingDir, timeStampForSpecialInBetweenFiles))
            {
                emit d("failed to loadAnyMissedFilesByRecursivelyScanningDirectoriesAndGiveThemThisTimestamp during our special in between case");
                return;
            }
#endif
            //Now I'm starting to think that if I do nothing it will work perfectly...

            //TODOreq: if more than 100 files are ever added (basically only ever in THIS state) that don't get put into the timestamp file, then i think it might cause it to TRY to pull from 'old' table every time and never be able to. or maybe is able to and doesn't matter because it has the same values... wasting cpu cycles only (idfk). Maybe I should just test the son of a bitch and see what happens :-P
        }
        else if(QFile::exists(absoluteActualWorkingDir + ".quickDirtyAutoBackupHalperDirStructure")) //now
        {
            if(!QFile::exists(absoluteActualWorkingDir + ".dirstructure.txt.old.from.tree.command") || QFile::exists(absoluteActualWorkingDir + "dirstructure.txt"))
            {
                emit d("fucked state. either .dirstructure.txt.old.from.tree.command doesn't exist or dirstructure.txt exists and shouldn't");
                return;
            }

            if(!heirarchyMolester.loadFromEasyTreeFile(absoluteActualWorkingDir, absoluteActualWorkingDir + ".quickDirtyAutoBackupHalperDirStructure", true))
            {
                emit d("failed to load .quickDirtyAutoBackupHalperDirStructure for heirarchy molester");
                return;
            }
        }
        else
        {
            emit d("couldn't determine state, must have not accounted for something");
            return;
        }

        heirarchyMolester.performHackyOccuranceRateMerging(100); //TODOreq: heirarchy molester needs to still function "normally" with[out] these hacks...
        //TODOreq: I'm prety sure that doing this merging 'before' stage 2/3 will solve the problem of the files added in stage 2 triggering the occurance rate threshold, but in the "old" table will be the files from stage 2/3 "last time/checkout". I don't think it will be an issue since we don't do occurance rate checking ON the old table (just pull from it), but I'm noting it so I double check later

        //MOLEST STAGE 2/3 -- Add files not in list (now table) but in dir to the table
        QDateTime timeStampForMissedFiles = QDateTime::fromString(commitIdTimestampAndMessage->commitDate, Qt::ISODate);
        timeStampForMissedFiles = timeStampForMissedFiles.addSecs(-1);
        if(!heirarchyMolester.loadAnyMissedFilesByRecursivelyScanningDirectoriesAndGiveThemThisTimestamp(absoluteActualWorkingDir, timeStampForMissedFiles)) //was tempted to put "ignore" lists in here, but that's for later... when re-creating timestamp file
        {
            emit d("failed to loadAnyMissedFilesByRecursivelyScanningDirectoriesAndGiveThemThisTimestamp");
            return;
        }

        //TODO LEFT OFF: need to do occurance rate analyzing and grab "most recent" if occurance rate is fucked.
        //Files added in stage 2/3 might hit that occurance rate threshold! FFFFF. Also need to decide what to do with renames.

        //MOLEST STAGE 3/3 -- USE TABLE
        if(!heirarchyMolester.molestUsingInternalTables()) //TODOreq: internal/merged table becomes "old" table for next run, at/when/after we call this
        {
            emit d("failed to molest");
            return;
        }


        //MAKE .gitgnore

        //regardless of if it's already made (might have 'checked out' contents), 'TOUCH' it to the earliest date ever (if(firstTimeTouching { genTouchTimestamp })) and keep re-touching each checkout to that same timestamp, so git final git repo doesn't record it ever changing
        if(!gitIgnoreFileGeneratedAndHeldInPlace.open(QIODevice::WriteOnly | QIODevice::Truncate)) //Not QIODevice::Text because idfk if git works with \r\n in gitignore (but shit this app isn't portable to windows anyways so wtf)
        {
            emit d("failed to open git ignore for writing");
            return;
        }
        foreach(QString gitIgnoreLine, gitIgnoreLines)
        {
            gitIgnoreTextStream << gitIgnoreLine << endl;
        }
        gitIgnoreFileGeneratedAndHeldInPlace.close();
        if(firstTimeCreatingGitIgnore)
        {
            gitIgnoreTimestamp = QDateTime::fromString(commitIdTimestampAndMessage->commitDate, Qt::ISODate);
            gitIgnoreTimestamp = gitIgnoreTimestamp.addSecs(-1); //one second before the first commit, we created our .gitignore ... and never modified (had:touched) it again
            firstTimeCreatingGitIgnore = false;
        }
        if(!fileModificationChanger.changeModificationDate(gitIgnoreFileGeneratedAndHeldInPlaceFilePath, gitIgnoreTimestamp))
        {
            emit d("failed to touch git ignore");
            return;
        }
#endif // CUSTOM_GIT_UNROLL_REROLL_HACKS_FOR_D3FAULT

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

#ifdef CUSTOM_GIT_UNROLL_REROLL_HACKS_FOR_D3FAULT

        //RE-CREATE TIMESTAMP FILE -- recursively iterate dir contents (do not use previous table from molest, as that has censored filepaths in it)

        //new one shouldn't have creation date or size [or hash]
        //should use "ignore" lists like QuickDirty does (perhaps with more (see gitIgnore))
        //Touch re-created timestamp file to commitTimestamp...

        //delete old timestamp formats first
        if(QFile::exists(absoluteActualWorkingDir + "dirstructure.txt"))
        {
            if(!QFile::remove(absoluteActualWorkingDir + "dirstructure.txt"))
            {
                emit d("failed to remove dirstructure.txt");
                return;
            }
        }
        if(QFile::exists(absoluteActualWorkingDir + ".dirstructure.txt.old.from.tree.command"))
        {
            if(!QFile::remove(absoluteActualWorkingDir + ".dirstructure.txt.old.from.tree.command"))
            {
                emit d("failed to remove .dirstructure.txt.old.from.tree.command");
                return;
            }
        }
        if(QFile::exists(absoluteActualWorkingDir + ".quickDirtyAutoBackupHalperDirStructure"))
        {
            if(!QFile::remove(absoluteActualWorkingDir + ".quickDirtyAutoBackupHalperDirStructure"))
            {
                emit d("failed to remove .quickDirtyAutoBackupHalperDirStructure");
                return;
            }
        }

        QFile newTimestampsFile(absoluteActualWorkingDir + ".lastModifiedTimestamps"); //woo, a new format xD (KILL ME (jk heirarchy molester exists so the problem that caused me to write THIS app is now dealt with (and molester was so easy to code facepalm.jpg :-/...)))
        if(!newTimestampsFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            emit d("failed to open .lastModifiedTimestamps for writing");
            return;
        }

        EasyTree easyTree;
        easyTree.generateTreeText(absoluteActualWorkingDir, &newTimestampsFile, false, &dirNamesToIgnore, &fileNamesToIgnore, &fileNamesEndWithIgnoreList, &dirNamesEndsWithIgnoreList, false);


        if(!newTimestampsFile.flush()) //flush is necessary or else the git add/commit might miss it...
        {
            emit d("failed to flush .lastModifiedTimestamps");
            return;
        }
        newTimestampsFile.close();

#endif // CUSTOM_GIT_UNROLL_REROLL_HACKS_FOR_D3FAULT

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


    //cleanup -- TODOreq: return;'ing anywhere above does not properly clean up! ffffff need me some scoped pointers, and probably custom deleters to first iterate the lists
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
}
