#ifndef GITBITSHITDAEMON_H
#define GITBITSHITDAEMON_H

#include <QObject>

class QLocalServer;

class GitBitShitDaemon : public QObject
{
    Q_OBJECT
public:
    explicit GitBitShitDaemon(QObject *parent = 0);
private:
    QLocalServer *m_LocalServer;

    void startGitBitShitForLatestCommitInTargetRepo();
    void handleInternalGitMetaRepoPostUpdateSignaled();
private slots:
    void handleLocalServerNewConnection();
    void processMessage(const QString &theMessage);
};

#endif // GITBITSHITDAEMON_H
