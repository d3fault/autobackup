#include "stupidkeyvaluecontenttrackercli.h"

#include <QCoreApplication>

StupidKeyValueContentTrackerCli::StupidKeyValueContentTrackerCli(QObject *parent)
    : QObject(parent)
    , m_StdErr(stderr)
    , m_StdOut(stdout)
{
    connect(this, &StupidKeyValueContentTrackerCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection);
}
void StupidKeyValueContentTrackerCli::main()
{
    //TODOreq: parse app args. ex: ./thisApp add key data
    emit addRequested("testKey0", "testData1");
}
void StupidKeyValueContentTrackerCli::handleE(QString msg)
{
    m_StdErr << msg << endl;
}
void StupidKeyValueContentTrackerCli::handleO(QString msg)
{
    m_StdOut << msg << endl;
}
void StupidKeyValueContentTrackerCli::handleAddFinished(bool success)
{
    if(success)
    {
        handleO("add finished successfully");
        emit commitRequested("testCommitMessage0");
    }
    else
    {
        handleE("add failed");
    }
}
void StupidKeyValueContentTrackerCli::handleCommitFinished(bool success)
{
    if(success)
    {
        handleO("commit finished successfully");
        QString revision;
        emit readKeyRequested("testKey0", revision);
    }
    else
    {
        handleE("commit failed");
    }
}
void StupidKeyValueContentTrackerCli::handleReadKeyFinished(bool success, QString key, QString revision, QString data)
{
    if(success)
    {
        handleO("readKeyFinished finished successfully");
        handleO(data);
    }
    else
    {
        handleE("readKeyFinished failed");
    }

    emit exitRequested(success ? 0 : 1);
}
