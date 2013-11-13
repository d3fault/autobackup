#include "githelper.h"

GitHelper::GitHelper(QObject *parent)
    : QObject(parent)
{
    m_GitProcess.setProcessChannelMode(QProcess::MergedChannels);
}
void GitHelper::gitClone(QString srcRepoAbsolutePath, QString destRepoAbsolutePath, QString optionalWorkTree)
{
    QStringList gitCloneArguments;
    if(!optionalWorkTree.isEmpty())
    {
        gitCloneArguments << QString("--work-tree=" + optionalWorkTree); //TODOreq: verify spaces in work-tree doesn't break
    }
    gitCloneArguments << "clone" << QString("file://" + srcRepoAbsolutePath) << destRepoAbsolutePath; //TODOreq: verify dest can work with absolute path. i've only ever done it with a single folder name, and it gets created in the cwd that way
    if(!m_GitProcess.waitForStarted())
    {
        emit d("failed to start git clone executable");
        return;
    }
    if(!m_GitProcess.waitForFinished())
    {
        emit d("git clone executable failed to finish");
        return;
    }
    int exitCode = m_GitProcess.exitCode();
    if(exitCode != 0)
    {
        emit d("git clone exit code was not zero");
        return;
    }
    QString gitCloneResultString = m_GitProcess.readAll();
    if(gitCloneResultString.trimmed().isEmpty())
    {
        emit d("git clone didn't say anything, so there was probably an error");
        return;
    }

    //TODOreq: we could probably do basic 'contains' verification here
    emit d(gitCloneResultString);
}
