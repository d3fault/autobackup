#include "githelper.h"

//I genuinely have no idea what the copyright of this would be (if GPL consumes it), _BUT_ since I most likely won't be distributing it in binary form, it doesn't matter and I can copyright it as proprietary if I damn well please (DPL most likely). GPL and DPL are two-way compatible if everything is distributed via source. Hell I think I could even link against GPL (shit isn't there a libgit I coulda used?!?!? Fuckit).
//Anyways my main reason for the above is because this is clearly "very fucking dependent on" (legal terminology) gpl/git, which means it is (or might be, idfk) "a part of" the gpl/git work

/*
  TODO LEFT OFF
fill out committer name (?????)
*/

const QString GitHelper::m_GitBinaryFilePath = "/usr/bin/git";

//TODOreq: apparently --work-tree isn't used if --git-dir isn't used, so all/most of my 'optional' git dir arguments should now be mandatory so as not to lead to undefined results [should someone else use this (or myself in the future)]. Basically this is a pretty non-portable app-specific helper I'm coding :(, but I can definitely use it in my masturbation auto git submodules app

GitHelper::GitHelper(QObject *parent)
    : QObject(parent)
{
    m_GitProcess.setProcessChannelMode(QProcess::MergedChannels);
}
void GitHelper::changeWorkingDirectory(QString newWorkingDirectory)
{
    m_GitProcess.setWorkingDirectory(newWorkingDirectory);
}
bool GitHelper::gitClone(QString srcRepoAbsolutePath, QString destRepoAbsolutePath, QString optionalWorkTree)
{
    QStringList gitCloneArguments;
    if(!optionalWorkTree.isEmpty())
    {
        gitCloneArguments << QString("--work-tree=" + optionalWorkTree); //TODOreq: verify spaces in work-tree doesn't break -- affects git init too
    }
    gitCloneArguments << "clone" << QString("file://" + srcRepoAbsolutePath) << destRepoAbsolutePath; //TODOreq: verify dest can work with absolute path. i've only ever done it with a single folder name, and it gets created in the cwd that way
    m_GitProcess.start(m_GitBinaryFilePath, gitCloneArguments);
    if(!m_GitProcess.waitForStarted())
    {
        emit d("failed to start git clone executable");
        return false;
    }
    if(!m_GitProcess.waitForFinished())
    {
        emit d("git clone executable failed to finish");
        return false;
    }
    QString gitCloneResultString = m_GitProcess.readAll();
    //TODOreq: we could probably do basic 'contains' verification here
    emit d(gitCloneResultString);
    int exitCode = m_GitProcess.exitCode();
    if(exitCode != 0)
    {
        emit d("git clone exit code was not zero");
        return false;
    }    
    if(gitCloneResultString.trimmed().isEmpty())
    {
        emit d("git clone didn't say anything, so there was probably an error");
        return false;
    }

    return true;
}
//If dirToInit is not specified, whatever working directory m_GitProcess is in when called is the directory that will be initialized. If detachedAssociatedGitFolderLocation is not specified, then the the git files will be in a ".git" subfolder in the dirToInit
bool GitHelper::gitInit(QString semiOptionaldirToInit, QString optionalDetachedAssociatedGitFolderLocation)
{
    QStringList gitInitArguments;
    if(!semiOptionaldirToInit.isEmpty())
    {
        gitInitArguments << QString("--work-tree=" + semiOptionaldirToInit);
    }
    if(!optionalDetachedAssociatedGitFolderLocation.isEmpty())
    {
        gitInitArguments << QString("--git-dir=" + optionalDetachedAssociatedGitFolderLocation);
    }
    gitInitArguments << "init";

    m_GitProcess.start(m_GitBinaryFilePath, gitInitArguments);
    if(!m_GitProcess.waitForStarted())
    {
        emit d("failed to start git init executable");
        return false;
    }
    if(!m_GitProcess.waitForFinished())
    {
        emit d("git init executable failed to finish");
        return false;
    }
    QString gitInitResultString = m_GitProcess.readAll();
    //TODOreq: we could probably do basic 'contains' verification here
    emit d(gitInitResultString);
    int exitCode = m_GitProcess.exitCode();
    if(exitCode != 0)
    {
        emit d("git init exit code was not zero");
        return false;
    }
    if(gitInitResultString.trimmed().isEmpty())
    {
        emit d("git init didn't say anything, so there was probably an error");
        return false;
    }

    return true;
}
//If dirToRunGitLogIn is not provided or is empty, the "git log" command will be run in m_GitProcess's current working directory. Caller takes owner of each item in the QList
bool GitHelper::gitLogReturningCommitIdsAuthorDateAndCommitMessage(QList<GitCommitIdTimestampAndMessage*> *commitIdsStringListToFill, QString semiOptionalDirToRunGitLogIn)
{
    QStringList gitLogArguments;
    if(!semiOptionalDirToRunGitLogIn.isEmpty())
    {
        changeWorkingDirectory(semiOptionalDirToRunGitLogIn);
#if 0
        gitLogArguments << "--git-dir=" << semiOptionalDirToRunGitLogIn; //TODOreq: I've not yet tested that this way of specifying a git-dir for use with git log even works, but it should. If it doesn't, I just need to change the working dir of the m_GitProcess
#endif
    }

    gitLogArguments << "log";
    gitLogArguments << "--format=format:%H%n%ai%n%s"; //woot fuck parsing <3 linus (not that this is any indication of his greatness (btw OpenBSD is bettar and I would be honored if he called me a masturbating monkey :-D. That's what life is all about~))
    //^^aww shit wrote that a while ago and forgot i need date and commit message :-/.. guess I need to parse after all, but still should be easy since I can 'define' my format so easily..
    //bah idfk if %s, %b, ,%B, and last/least-likely %N is what I want for getting a commit message :-/...
    //Maybe I should use a "MAGIC" separator between ID/timestamp/message instead of a newline, because what if a message has a newline :-S. EVEN IF I DO USE 'MAGIC', commit messages might still have a newline which fucks up with how I currently parse. So I need a message start indicator magic too, OH BOY DOES THIS SOUND FUCKING FAMILIAR SFOUSDFLKJASDOUFALSKJDOUW
    //Woot love being an h4x0r: `git log --format=format:%H | wc -l` == `git log --format=format:%s | wc -l`
    //OT: mfw my QuickDirty timestamps are just a couple of variable amount of seconds ahead of the git commit date: however long the git add + commit took (could fix this but idgaf)

    m_GitProcess.start(m_GitBinaryFilePath, gitLogArguments);
    if(!m_GitProcess.waitForStarted())
    {
        emit d("failed to start git log executable");
        return false;
    }
    if(!m_GitProcess.waitForFinished())
    {
        emit d("git log executable failed to finish");
        return false;
    }
    int exitCode = m_GitProcess.exitCode();
    if(exitCode != 0)
    {
        emit d("git log exit code was not zero");
        return false;
    }
    QString gitLogResultString = m_GitProcess.readAll();
    if(gitLogResultString.trimmed().isEmpty())
    {
        emit d("git log didn't say anything, so there was probably an error");
        return false;
    }

    //GPL vs. LGPL/DPL -- can still do all the same shit, but you gotta parse output instead of being able to just use the programming objects -_- (my understanding might be faulty (correct me plz))
    QTextStream textStream(&gitLogResultString, QIODevice::ReadOnly | QIODevice::Text);
    int commitCount = 0;
    while(!textStream.atEnd())
    {
        QString currentLine = textStream.readLine();
        if(!currentLine.trimmed().isEmpty())
        {
            //very basic ass sanity checking
            if(currentLine.length() != 40)
            {
                emit d("we were expecting a 40 character sha1sum but didn't see it. saw this instead: " + currentLine);
                return false;
            }

            //commitIdsStringListToFill->append(currentLine);
            GitCommitIdTimestampAndMessage *commitIdTimestampAndMessage = new GitCommitIdTimestampAndMessage();
            commitIdTimestampAndMessage->commitId = currentLine;

            currentLine = textStream.readLine();
            if(currentLine.isEmpty())
            {
                emit d("error: found an empty line in git log when expecting a timestamp");
                return false;
            }
            commitIdTimestampAndMessage->commitDate = currentLine;

            currentLine = textStream.readLine();
            if(currentLine.isEmpty())
            {
                emit d("error: found an empty line in git log when expecting a commit message");
                return false;
            }
            commitIdTimestampAndMessage->commitMessage = currentLine;


            commitIdsStringListToFill->append(commitIdTimestampAndMessage);
            ++commitCount;
        }
    }
    if(commitCount < 1)
    {
        emit d("didn't find any commits after running git log, so wtf?");
        return false;
    }
    emit d("commits in censored repo to iterate: " + QString::number(commitCount));

    return true;
}
//If semiOptionalWorkTree isn't specified, m_GitProcess's current working directory is where the git checkout is run
bool GitHelper::gitCheckout(QString commitIdToCheckout, QString semiOptionalWorkTree, QString optionalDetachedAssociatedGitFolderLocation)
{
    QStringList gitCheckoutArgs;
    if(!semiOptionalWorkTree.isEmpty())
    {
        gitCheckoutArgs << QString("--work-tree=" + semiOptionalWorkTree);
    }
    if(!optionalDetachedAssociatedGitFolderLocation.isEmpty())
    {
        gitCheckoutArgs << QString("--git-dir=" + optionalDetachedAssociatedGitFolderLocation);
    }
    gitCheckoutArgs << "checkout" << commitIdToCheckout;

    m_GitProcess.start(m_GitBinaryFilePath, gitCheckoutArgs);
    if(!m_GitProcess.waitForStarted())
    {
        emit d("failed to start git checkout executable");
        return false;
    }
    if(!m_GitProcess.waitForFinished())
    {
        emit d("git checkout executable failed to finish");
        return false;
    }
    QString gitCheckoutResultString = m_GitProcess.readAll();
    //TODOreq: we could probably do basic 'contains' verification here
    emit d(gitCheckoutResultString);
    int exitCode = m_GitProcess.exitCode();
    if(exitCode != 0)
    {
        emit d("git checkout exit code was not zero");
        return false;
    }   
    if(gitCheckoutResultString.trimmed().isEmpty())
    {
        emit d("git checkout didn't say anything, so there was probably an error");
        return false;
    }

    return true;
}
//This doesn't provide all the functionality that git add does/should, so you should probably code your own git add helper ;-). This one just adds all the files in pathToRunGitAddIn and allows you to [optionally] specify an associated detached git dir, BUT DOES NOT let you specify specific files etc as one would expect. Also, the pathToRunGitAddIn is not only cd'd into, but also specified in the git add command as the --work-tree , which may or may not fuck with your results (SUITS MINE PERFECTLY KTHXBAI)
//'specify specific' xD
bool GitHelper::gitAdd(QString pathToRunGitAddIn, QString optionalDetachedAssociatedGitFolderLocation)
{
    changeWorkingDirectory(pathToRunGitAddIn);

    QStringList gitAddArguments;
    gitAddArguments << QString("--work-tree=" + pathToRunGitAddIn);
    if(!optionalDetachedAssociatedGitFolderLocation.isEmpty())
    {
        gitAddArguments << QString("--git-dir=" + optionalDetachedAssociatedGitFolderLocation);
    }
    gitAddArguments << "add" /*<< "-v"*/ << ".";

    m_GitProcess.start(m_GitBinaryFilePath, gitAddArguments);
    if(!m_GitProcess.waitForStarted())
    {
        emit d("failed to start git add executable");
        return false;
    }
    if(!m_GitProcess.waitForFinished())
    {
        emit d("git add executable failed to finish");
        return false;
    }
    QString gitAddResultString = m_GitProcess.readAll();
    int exitCode = m_GitProcess.exitCode();
    if(exitCode != 0)
    {
        emit d("git add results: " + gitAddResultString);
        emit d("git add exit code was not zero");
        return false;
    }
    if(!gitAddResultString.trimmed().isEmpty())
    {
        emit d("git add said something, so there was probably an error"); //or we could uncomment the -v and verify that all the adds were successful...
        emit d("git add results: " + gitAddResultString);
        return false;
    }

    return true;
}
bool GitHelper::gitCommit(QString workTree, QString gitDir, QString commitMessage, QString commitDate, QString author)
{
    QStringList gitCommitArguments;
    gitCommitArguments << QString("--work-tree=" + workTree) << QString("--git-dir=" + gitDir) << "commit" << QString("--date=" + commitDate) << QString("--author=" + author) << "-am" << commitMessage;

    m_GitProcess.start(m_GitBinaryFilePath, gitCommitArguments);
    if(!m_GitProcess.waitForStarted())
    {
        emit d("failed to start git commit executable");
        return false;
    }
    if(!m_GitProcess.waitForFinished())
    {
        emit d("git commit executable failed to finish");
        return false;
    }
    int exitCode = m_GitProcess.exitCode();
    QString gitCommitResultString = m_GitProcess.readAll();
    //TODOreq: we could probably do basic 'contains' verification here
    emit d(gitCommitResultString);
    if(exitCode != 0)
    {
        if(gitCommitResultString.contains("nothing to commit (working directory clean)"))
        {
            emit d("commit message lost because censored file was only change in commit: " + commitMessage);
            return true;
        }
        emit d("git commit exit code was not zero");
        return false;
    }
    if(gitCommitResultString.trimmed().isEmpty())
    {
        emit d("git commit didn't say anything, so there was probably an error");
        return false;
    }

    return true;
}
