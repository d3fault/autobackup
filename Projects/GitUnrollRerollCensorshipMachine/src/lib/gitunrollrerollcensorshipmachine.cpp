#include "gitunrollrerollcensorshipmachine.h"

#define CUSTOM_GIT_UNROLL_REROLL_HACKS_FOR_D3FAULT 1

#ifdef CUSTOM_GIT_UNROLL_REROLL_HACKS_FOR_D3FAULT
#include <QProcess>
#include "filemodificationdatechanger.h"
#include "lastmodifieddateheirarchymolester.h"
#include "easytree.h"
#endif

struct QListGitCommitIdTimestampAndMessageDeleter
{
    static inline void cleanup(QList<GitCommitIdTimestampAndMessage*> *pointer)
    {
        int listSize = pointer->size();
        for(int i = 0; i < listSize; ++i)
        {
            GitCommitIdTimestampAndMessage *item = pointer->at(i);
            delete item;
        }
        delete pointer;
    }
};
struct QListEasyTreeHashItemDeleter
{
    static inline void cleanup(QList<EasyTreeHashItem*> *pointer)
    {
        int listSize = pointer->size();
        for(int i = 0; i < listSize; ++i)
        {
            EasyTreeHashItem *item = pointer->at(i);
            delete item;
        }
        delete pointer;
    }
};

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
    QScopedPointer<QList<EasyTreeHashItem*>, QListEasyTreeHashItemDeleter> easyTreeHashItemListOfFilesToCensor(EasyTreeParser::parseEasyTreeAndReturnAsNewList(&fileWithListOfFilePathsToCensor));
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
    QScopedPointer<QList<GitCommitIdTimestampAndMessage*>, QListGitCommitIdTimestampAndMessageDeleter> allCommitSha1sFromGitLogCommand(new QList<GitCommitIdTimestampAndMessage*>());
    if(!m_GitHelper.gitLogReturningCommitIdsAuthorDateAndCommitMessage(allCommitSha1sFromGitLogCommand.data(), absoluteCensoredSourceDirsAssociatedDetachedGitFolder))
    {
        emit d("failed to run git log in order to iterate the history of the censored repo");
        return;
    }

#ifdef CUSTOM_GIT_UNROLL_REROLL_HACKS_FOR_D3FAULT
    QDateTime executionDateTime = QDateTime::currentDateTime(); //TODOreq: in UTC method instead? man whoever invented timezones and daylight savings time should be shot

    QString gitIgnoreFileGeneratedAndHeldInPlaceFilePath = absoluteActualWorkingDir + ".gitignore";
    QFile gitIgnoreFileGeneratedAndHeldInPlace(gitIgnoreFileGeneratedAndHeldInPlaceFilePath);
    QStringList gitIgnoreLines;
    gitIgnoreLines << "*.[ao]" << "*.autosave" << "*.kate-swp" << "*.pro.user" << "*~" << "*__Qt_SDK__Debug/" << "*__Qt_SDK__Release/" << "*_in_PATH__System__Debug/" << "*_in_PATH__System__Release/";
    QTextStream gitIgnoreTextStream(&gitIgnoreFileGeneratedAndHeldInPlace);
    FileModificationDateChanger fileModificationChanger;
    bool firstTimeCreatingGitIgnore = true;
    QDateTime gitIgnoreTimestamp;

    //we escape the filename sent to find in case it has a space, and since we're using the string execute method
    QString findChmodCommand = "/usr/bin/find \"" + absoluteActualWorkingDir + "\"" + " ( -type d -exec /bin/chmod 755 {} ; ) -o ( -type f -exec /bin/chmod 644 {} ; )"; //took me longer than expected to figure out why `chmod -R 644 dir/` wouldn't work xD (stupid coders)

    LastModifiedDateHeirarchyMolester heirarchyMolester;
    connect(&heirarchyMolester, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    QList<QString> dirNamesToIgnore;
    dirNamesToIgnore.append(".git");
    QList<QString> fileNamesToIgnore;
    fileNamesToIgnore.append(".lastModifiedTimestamps"); //hack, ignore own output xD
    QList<QString> fileNamesEndWithIgnoreList;
    fileNamesEndWithIgnoreList.append(".pro.user");
    fileNamesEndWithIgnoreList.append("~");
    fileNamesEndWithIgnoreList.append(".a");
    fileNamesEndWithIgnoreList.append(".o");
    fileNamesEndWithIgnoreList.append(".autosave");
    fileNamesEndWithIgnoreList.append(".kate-swp");
    QList<QString> dirNamesEndsWithIgnoreList;
    dirNamesEndsWithIgnoreList.append("__Qt_SDK__Debug");
    dirNamesEndsWithIgnoreList.append("__Qt_SDK__Release");
    dirNamesEndsWithIgnoreList.append("_in_PATH__System__Debug");
    dirNamesEndsWithIgnoreList.append("_in_PATH__System__Release");

    bool skipTheRestOfCustomHacksThisIteration = false;
#endif

    //iterate over the git log backwards, because we want to start from the beginning/oldest commit
    int censoredRepoCommitsCount = allCommitSha1sFromGitLogCommand->size(); //TODOreq: verify this commit count with our destination/censored commit count
    for(int i = censoredRepoCommitsCount-1; i > -1; --i) //TODOreq: verify not off by one error
    {
        emit d("commits left: " + QString::number(i)); //this might be off by one error, but idgaf

        GitCommitIdTimestampAndMessage *commitIdTimestampAndMessage = allCommitSha1sFromGitLogCommand->at(i);
        QString currentCommitId = commitIdTimestampAndMessage->commitId;

        //I still wonder if a "rm -rfv ./*" (err, able to get hidden files tho) right here before the force checkout would make the end result cleaner

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
        if(findChmodReturnCode != 0)
        {
            emit d("find chmod command didn't return 0: " + QString::number(findChmodReturnCode));
            return;
        }

        //MOLEST STAGE 1/3 -- FILL TABLE

        //TODOreq: we need to verify that all the files in the heirarchy were molested (maybe not all are in the list), otherwise it'll have a timestamp at the time of execution. If it wasn't molested, give it a sane timestamp (checkout-1, for example)

        skipTheRestOfCustomHacksThisIteration = false;

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

            //bah, back in my old dirstructure.txt days i'd sometimes manually commit just so i could write a commit message... but doing so didn't update the timestamp file (hence QuickDirty)... so now my "last check before molest" is seeing deleted/renamed-from files in it's "table" because of that fact. how to deal with :-/
            if(!commitIdTimestampAndMessage->commitMessage.startsWith("Auto-Commit @"))
            {
                //MUAHAHAHAHAHAHAHA h4x0r4life
                //check internal tables, and for entries not existing, just drop them instead of erroring out :)
                heirarchyMolester.dropNonExistingEntries();
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


            //This is a guess at a hack that may or may not work (my head hurts + lazy):
            if(!heirarchyMolester.loadFromXml(absoluteActualWorkingDir, absoluteActualWorkingDir + ".dirstructure.txt.old.from.tree.command"))
            {
                emit d("failed to load .dirstructure.txt.old.from.tree.command for heirarchy molester");
                return;
            }

            if(!commitIdTimestampAndMessage->commitMessage.startsWith("Auto-Commit @"))
            {
                heirarchyMolester.dropNonExistingEntries();
            }
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
        else // ancient, no timestamp file
        {
            //mfw there's 6 months of commits with no dir structure file i had forgotten existed, guess i gotta just use the commit date... I don't think I have it in me tonight...
            //TODO LEFT OFF

            //TODOreq: I tried, since I had my testbox already running, putting a .loadAnyMissedFiles[..] in here using commit-1 as the date, and I think I found a bug that my "table" shit doesn't handle deleted directories. BUT maybe that's just because I threw that hack in here for testing??? 'clear'ing the current tables before running .loadAny seems to have gotten past that bug codewise, but I'm not sure if the logic checks out. Also definitely seeing a lot of "fucked state XYZ" things (shit's still running). Those XYZ errors might be because I cleared/loaded hackily _HERE_ (but only on test machine). It's funny watching it go... each commit takes longer and longer xD

            //TODOreq: It's _VERY_ likely that these ancient commits might go over the threshold multiple times. If I have any commits with > 100 files, those will all have the same checked out time. Should I dynamically raise the threshold for these then? More importantly, does it even matter? All these intersecting paths of logic are hurting my brain, I'm not far from just releasing the book without [all] timestamps xD. Knowing they're in 'special' would be good enough for me :-P

            //WHY THE FUCK am I stumped on what to do here? Is it because I don't have an easy way (or dunno how lol) to see if a file changed with a commit so it should take on the commitDate-1 of the new commit (instead of always retaining the commitDate-1 for the first time it was seen)? This is starting to piss me off and make me feel like I'll never launch. Makes me want to say fuck this whole app and go with no timestamps. ALL THIS CODE and I get stumped on something so stupid/simple.. xD. I've been staring at the screen thinking for like 20 minutes now... wtf. I'd be happy with a "good enough" simple solution, but I can't think of one..

            //Here's an easy solution: give them all commitDate-1 for the very last commit before dirstructure.txt was introduced. WHO GIVES A FUCK. I think that would amount to a "continue;" here except for that last one. Or hell maybe just even the first time dirstructure.txt gets them is good enough imo fuck it (IN FACT, IT MIGHT CONTAIN SEMI-ACTUAL (OR ACTUAL) VALUES!?!?)). Wtfz
            //Err actually it wouldn't be a continue, because then we'd never add/commit xD

            if(!QFile::exists(absoluteActualWorkingDir + "dirstructure.txt") && !QFile::exists(absoluteActualWorkingDir + ".dirstructure.txt.old.from.tree.command") && !QFile::exists(absoluteActualWorkingDir + ".quickDirtyAutoBackupHalperDirStructure"))
            {
                skipTheRestOfCustomHacksThisIteration = true;
            }
            else
            {
                emit d("couldn't determine state, must have not accounted for something");
                return;
            }
        }

        if(!skipTheRestOfCustomHacksThisIteration)
        {
            heirarchyMolester.performHackyOccuranceRateMerging(100); //TODOreq: heirarchy molester needs to still function "normally" with[out] these hacks...
            //TODOreq: I'm prety sure that doing this merging 'before' stage 2/3 will solve the problem of the files added in stage 2 triggering the occurance rate threshold, but in the "old" table will be the files from stage 2/3 "last time/checkout". I don't think it will be an issue since we don't do occurance rate checking ON the old table (just pull from it), but I'm noting it so I double check later

            //TODOreq: either here or before perform[...] (unsure tbh, might not matter), I need to do a QFile::exists on every entry in the tables and remove non-existent ones. This is to account for deletes/renames between commits
            //A way to test this is to make sure there are no files of size 0 (touch would create them (but luckily errors out if the dir the file attempting to touch doesn't exist))
            //^^^^^^^I think this was only because I was doing "loadAnyMissed" on the "ancient" (no timestamp) shit. The proper loads from timestamp files WILL handle deletes/renames...


            //MOLEST STAGE 2/3 -- Add files not in list (now table) but in dir to the table
            QDateTime timeStampForMissedFiles = QDateTime::fromString(commitIdTimestampAndMessage->commitDate, Qt::ISODate);
            timeStampForMissedFiles = timeStampForMissedFiles.addSecs(-1);
            if(!heirarchyMolester.loadAnyMissedFilesByRecursivelyScanningDirectoriesAndGiveThemThisTimestamp(absoluteActualWorkingDir, timeStampForMissedFiles)) //was tempted to put "ignore" lists in here, but that's for later... when re-creating timestamp file
            {
                emit d("failed to loadAnyMissedFilesByRecursivelyScanningDirectoriesAndGiveThemThisTimestamp");
                return;
            }

            //Files added in stage 2/3 might hit that occurance rate threshold! FFFFF. Also need to decide what to do with renames.



            heirarchyMolester.removeFilePathsFromTablesThatTimestampFileGotButGitIgnored(fileNamesEndWithIgnoreList, dirNamesEndsWithIgnoreList);

            //TODOreq: as a very last step before molest, I can check the internal tables don't have a timestamp >= execution date (meaning I missed them) -- but meh isn't this already accounted for in loadAny?
            if(!heirarchyMolester.allFilePathsInCurrentTableExistAndDontHaveDateTimeGreaterThanOrEqualTo(executionDateTime))
            {
                emit d("a file path about to be molested didn't exist, or had a datetime greater than our execution time");
                return;
            }


            //MOLEST STAGE 3/3 -- USE TABLE
            if(!heirarchyMolester.molestUsingInternalTables()) //TODOreq: internal/merged table becomes "old" table for next run, at/when/after we call this
            {
                emit d("failed to molest");
                return;
            }
        }


        //even when skipTheRestOfCustomHacksThisIteration is true, we still want to do the .gitignore hack :)
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
        if(!gitIgnoreFileGeneratedAndHeldInPlace.flush())
        {
            emit d("failed to flush gitIgnoreFileGeneratedAndHeldInPlace");
            return;
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

        if(!skipTheRestOfCustomHacksThisIteration)
        {
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
        }

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

    //TODOreq: CLONE TEMP CENSORED DESTINATION TO USER SUPPLIED DESTINATION -- especially since IIRC there will be some files in there we want to not give a fuck about
}
