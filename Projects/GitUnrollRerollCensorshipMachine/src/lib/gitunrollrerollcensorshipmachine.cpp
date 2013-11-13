#include "gitunrollrerollcensorshipmachine.h"

GitUnrollRerollCensorshipMachine::GitUnrollRerollCensorshipMachine(QObject *parent) :
    QObject(parent)
{
    connect(&m_GitHelper, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
void GitUnrollRerollCensorshipMachine::unrollRerollGitRepoCensoringAtEachCommit(QString filePathToListOfFilepathsToCensor, QString sourceGitDirToCensor, QString destinationGitDirCensored, QString workingDir)
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
    if(!QFile::exists(sourceGitDirToCensor))
    {
        emit d("folder does not exist: " + sourceGitDirToCensor);
        return;
    }
    if(!QFile::exists(destinationGitDirCensored))
    {
        emit d("folder does not exist: " + destinationGitDirCensored);
        return;
    }
    if(!QFile::exists(workingDir))
    {
        emit d("folder does not exist: " + workingDir);
        return;
    }

    if(!fileWithListOfFilePathsToCensor.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("failed to open for reading: " + filePathToListOfFilepathsToCensor);
        return;
    }
    QList<EasyTreeHashItem*> *listOfFilesToCensor = EasyTreeParser::parseEasyTreeAndReturnAsNewList(&fileWithListOfFilePathsToCensor);

    m_GitHelper.gitClone(sourceGitDirToCensor, destinationGitDirCensored, workingDir);
    //my mind exploded here when i realized that no, you should not return a bool from a slot because "cross thread calls" (signals/slots) mimic the cross network paradigm... and it would be fucking stupid if a computer you were talking to over the network error'd out which caused you to error out. still, wtf is the 'proper design' to not continue if the git clone fails!?!? saving and going to sleep

    //cleanup
    foreach(EasyTreeHashItem* item, listOfFilesToCensor)
    {
        delete item;
    }
    delete listOfFilesToCensor;

    //2.0 custom hacks
}
