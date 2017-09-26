#include "stupidkeyvaluecontenttrackercli.h"

#include <QCoreApplication>

StupidKeyValueContentTrackerCli::StupidKeyValueContentTrackerCli(QObject *parent)
    : QObject(parent)
{
    connect(this, &StupidKeyValueContentTrackerCli::exitRequested, qApp, &QCoreApplication::exit);
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
        handleO("add finished");
    }
    else
    {
        handleE("add failed");
    }

    emit exitRequested(success ? 0 : 1);
}
void StupidKeyValueContentTrackerCli::handleCommitFinished(bool success)
{
    //TODOstub
    qWarning("stub not implemented: StupidKeyValueContentTrackerCli::handleCommitFinished(bool success)");
}
void StupidKeyValueContentTrackerCli::handleReadKeyFinished(bool success, QString key, QString revision, QString data)
{
    //TODOstub
    qWarning("stub not implemented: StupidKeyValueContentTrackerCli::handleReadKeyFinished(bool success, QString key, QString revision, QString data)");
}
