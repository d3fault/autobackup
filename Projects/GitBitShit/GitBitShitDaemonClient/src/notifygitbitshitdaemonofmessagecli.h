#ifndef NOTIFYGITBITSHITDAEMONOFMESSAGECLI_H
#define NOTIFYGITBITSHITDAEMONOFMESSAGECLI_H

#include <QObject>
#include <QtNetwork/QLocalSocket>

class NotifyGitBitShitDaemonOfNotifyGitBitShitDaemonOfMessageCli : public QObject
{
    Q_OBJECT
public:
    explicit NotifyGitBitShitDaemonOfNotifyGitBitShitDaemonOfMessageCli(QObject *parent = 0);
private:
    QLocalSocket *m_LocalSocket;
    int m_CommandNumber;

    void usageAndQuit();
    void sendCommandToDaemonByNumber(int commandNumber);
    void notifyDaemonOfMessage(const QString &theMessage);
private slots:
    void handleLocalSocketStateChanged(QLocalSocket::LocalSocketState localSocketState);
};

#endif // NOTIFYGITBITSHITDAEMONOFMESSAGECLI_H
