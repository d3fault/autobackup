#ifndef GITBITSHITDAEMONCLIENT_H
#define GITBITSHITDAEMONCLIENT_H

#include <QObject>

class QLocalSocket;

class GitBitShitDaemonClient : public QObject
{
    Q_OBJECT
public:
    explicit GitBitShitDaemonClient(QLocalSocket *client, QObject *parent = 0);
    void disconnectClient();
    ~GitBitShitDaemonClient();
private:
    QLocalSocket *m_Client;
signals:
    void messageReceived(const QString &theMessage);
    void clientDisconnected(GitBitShitDaemonClient *self);
private slots:
    void handleReadyRead();
    void handleClientDisconnected();
};

Q_DECLARE_METATYPE(GitBitShitDaemonClient*)

#endif // GITBITSHITDAEMONCLIENT_H
