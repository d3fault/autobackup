#ifndef GITBITSHITDAEMON_H
#define GITBITSHITDAEMON_H

#include <QObject>
#include <QList>

class QLocalServer;

class GitBitShitDaemonClient;

class GitBitShitDaemon : public QObject
{
    Q_OBJECT
public:
    explicit GitBitShitDaemon(QObject *parent = 0);
    ~GitBitShitDaemon();
private:
    QLocalServer *m_LocalServer;
    QList<GitBitShitDaemonClient*> m_CurrentlyConnectedClients;

    void startGitBitShitForLatestCommitInTargetRepo();
    void handleInternalGitMetaRepoPostUpdateSignaled();
private slots:
    void handleLocalServerNewConnection();
    void processMessage(const QString &theMessage);
    void handleClientDisconnected(GitBitShitDaemonClient *disconnectedClient);
};

#endif // GITBITSHITDAEMON_H
