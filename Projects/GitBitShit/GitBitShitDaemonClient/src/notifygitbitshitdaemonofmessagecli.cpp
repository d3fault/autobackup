#include "notifygitbitshitdaemonofmessagecli.h"

#include <QDataStream>
#include <QTextStream>
#include <QCoreApplication>

#include "gitbitshitshared.h"

NotifyGitBitShitDaemonOfNotifyGitBitShitDaemonOfMessageCli::NotifyGitBitShitDaemonOfNotifyGitBitShitDaemonOfMessageCli(QObject *parent)
    : QObject(parent)
{
    QStringList argz = QCoreApplication::arguments();
    if(argz.size() < 2)
    {
        usageAndQuit();
        return;
    }

    QString commandNumberString = argz.at(1);
    bool commandNumberParseSuccess = false;
    int commandNumber = commandNumberString.toInt(&commandNumberParseSuccess);
    if(!commandNumberParseSuccess || commandNumber < 0 || commandNumber > 2)
    {
        usageAndQuit();
        return;
    }

    m_CommandNumber = commandNumber;

    m_LocalSocket = new QLocalSocket(this);
    connect(m_LocalSocket, SIGNAL(stateChanged(QLocalSocket::LocalSocketState)), this, SLOT(handleLocalSocketConnected(QLocalSocket::LocalSocketState)));
    m_LocalSocket->connectToServer(GitBitShitDaemonName, QLocalSocket::WriteOnly);
}
void NotifyGitBitShitDaemonOfNotifyGitBitShitDaemonOfMessageCli::usageAndQuit()
{
    QTextStream stdOut(stdout);
    stdOut << "Usage: NotifyGitBitShitDaemonOfInternalMetaRepoUpdateCli command-number\n\nCommand Numbers:\n\t0 - " TargetRepoUpdate_Message ": Use this command when your target repo has been committed to. Git's post-update script should call this, ideally.\n\t1 - " InternalMetaRepoUpdate_Message ": GitBitShit uses this command internally to notify the GitBitShitDameon when an internal repo has changed contents (which when an infohash of the target repo is updated)\n\t2 - Tell daemon to exit" << endl;
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
void NotifyGitBitShitDaemonOfNotifyGitBitShitDaemonOfMessageCli::sendCommandToDaemonByNumber(int commandNumber)
{
    if(commandNumber == 0)
    {
        notifyDaemonOfMessage(TargetRepoUpdate_Message);
        return;
    }
    if(commandNumber == 1)
    {
        notifyDaemonOfMessage(InternalMetaRepoUpdate_Message);
        return;
    }
    if(commandNumber == 3)
    {
        notifyDaemonOfMessage(StopGitBitShitDaemon_Message);
    }
}
void NotifyGitBitShitDaemonOfNotifyGitBitShitDaemonOfMessageCli::notifyDaemonOfMessage(const QString &theMessage)
{
    {
        QDataStream localSocketDataStream(static_cast<QIODevice*>(m_LocalSocket));
        localSocketDataStream << theMessage;
    }

    m_LocalSocket->flush();
    m_LocalSocket->close();
}
void NotifyGitBitShitDaemonOfNotifyGitBitShitDaemonOfMessageCli::handleLocalSocketStateChanged(QLocalSocket::LocalSocketState localSocketState)
{
    switch(localSocketState)
    {
    case QLocalSocket::ConnectedState:
        sendCommandToDaemonByNumber(m_CommandNumber);
        //notifyDaemonOfInternalMetaRepoUpdate();
        break;
    case QLocalSocket::ClosingState:
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
        break;
    default:
        break;
    }
}
