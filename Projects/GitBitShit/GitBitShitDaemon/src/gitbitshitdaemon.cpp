#include "gitbitshitdaemon.h"

#include <QSettings>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QTextStream>
#include <QCoreApplication>
#include <QDataStream>

#include "gitbitshitshared.h"
#include "gitbitshitdaemonclient.h"

GitBitShitDaemon::GitBitShitDaemon(QObject *parent)
    : QObject(parent)
{
    m_LocalServer = new QLocalServer(this);
    bool listening = false;
    if(!m_LocalServer->listen(GitBitShitDaemonName))
    {
        if(QLocalServer::removeServer(GitBitShitDaemonName))
        {
            if(m_LocalServer->listen(GitBitShitDaemonName))
            {
                listening = true;
            }
        }
    }
    else
    {
        listening = true;
    }

    if(!listening)
    {
        QTextStream stdErr(stderr);
        stdErr << "GitBitShitDaemon failed to start. the local server failed to listen by name '" GitBitShitDaemonName "'" << endl;
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    QTextStream stdOut(stdout);
    stdOut << "GitBitShitDaemon started" << endl;

    connect(m_LocalServer, SIGNAL(newConnection()), this, SLOT(handleLocalServerNewConnection()));
}
GitBitShitDaemon::~GitBitShitDaemon()
{
    Q_FOREACH(GitBitShitDaemonClient *currentClient, m_CurrentlyConnectedClients)
    {
        disconnect(currentClient, SIGNAL(clientDisconnected(GitBitShitDaemonClient*)), 0, 0); //so we don't double delete the GitBitShitDaemonClient
        currentClient->disconnectClient();
        currentClient->deleteLater();
    }
}
void GitBitShitDaemon::startGitBitShitForLatestCommitInTargetRepo()
{
    /*
        pseudo:
        git diff HEAD^ HEAD --patch-format=binary --include-git-commit-messages=yesplz > /temp/gitRawDiff-BEFOREHASH-AFTERHASH.bin;
        QString infoHash = m_LibTorrent.startSeedingTorrent("/temp/gitRawDiff-BEFOREHASH-AFTERHASH.bin");

        actual:
        NVM: git format-patch --minimal --binary --stdout HEAD~..HEAD > myPatchForLibTorrent

        QByteArray binaryPatchBundle = QProcess("git bundle create - HEAD~..HEAD").readAll();
    */
    QString infoHash;
    QSettings localMetaFile("/run/shm/tits.ini", QSettings::IniFormat);
    localMetaFile.setValue(InfoHashOf_LatestCommitDiff_Key, infoHash);
    /*
        git add .; git commit -am "blah meta update at <time>"; git push node0; git push node1; etc; #the git push also trigger this app, but now in command id #1 mode and on a different comp, which starts libtorrent as leech for the given infohash
    */
}
void GitBitShitDaemon::handleInternalGitMetaRepoPostUpdateSignaled()
{
    QString theMetaFileTellingUsAboutTheInfoHashOfTheNewRawGitCommit = "/run/shm/tits.ini"; //which itself (the meta file) just changed during a new commit (2 git repos in discussion)
    QSettings theMetaFile(theMetaFileTellingUsAboutTheInfoHashOfTheNewRawGitCommit, QSettings::IniFormat); //settings simply provide an easy api
    QString infoHashOf_LatestCommitDiff = theMetaFile.value(InfoHashOf_LatestCommitDiff_Key).toString();
    //now that we have the latest commit id, we can ask for a range to git diff. most likely it is the HEAD-1 to HEAD that will make up the contents of the diff, but we could provide infohashes for every combination of every git diff. there's an ever increasing amount of combinations, but we only need to store the infohash of the git diff, since the actual git diff contents can be re-generated on demand. pretending as though we have the file (and indeed, we do) lets us rely on libtorrent's auto manager to query the dht to find the least seeded "git diff ranges" and to seed those ones. Not every git diff range combination will be requested, but in order for this to work we need to at least generate every combination at least once. You know, maybe I should just say fuck commit combining (for now?). But yea I really want the ability to send an entire git history as one git diff and one torrent. Guh I need to decide how to do this. Nah for now I am going to require that you already have HEAD-1. I'll work something out smarter later.

    //myPatchForLibTorrent = m_LibTorrent.downloadFileByMagnet(infoHashOf_LatestCommitDiff);
    //NVM: QProcess("git am").write(myPatchForLibTorrent);

    //Optional: git bundle verify
    //QProcess("git fetch").write(binaryPatchBundle);
    //QProcess("git pull"); //fetch doesn't put it in the current tree, but pull does
}
void GitBitShitDaemon::handleLocalServerNewConnection()
{
    QLocalSocket *newConnectionActual = m_LocalServer->nextPendingConnection();
    GitBitShitDaemonClient *newConnection = new GitBitShitDaemonClient(newConnectionActual, this);
    connect(newConnection, SIGNAL(messageReceived(QString)), this, SLOT(processMessage(QString)));
    connect(newConnectionActual, SIGNAL(disconnected()), newConnection, SLOT(handleClientDisconnected()));
    connect(newConnection, SIGNAL(clientDisconnected(GitBitShitDaemonClient*)), this, SLOT(handleClientDisconnected(GitBitShitDaemonClient*)));
    m_CurrentlyConnectedClients.append(newConnection);
}
void GitBitShitDaemon::processMessage(const QString &theMessage)
{
    if(theMessage == TargetRepoUpdate_Message)
    {
        startGitBitShitForLatestCommitInTargetRepo();
        return;
    }
    if(theMessage == InternalMetaRepoUpdate_Message)
    {
        handleInternalGitMetaRepoPostUpdateSignaled(); //QHash<QString /*message*/, boost::function _OR_ QPair<QObject* /*callback receiver*/, QString /*callbackSlot on callback receiver*/>. same shit though really, although i bet hash lookups are faster than these if/else or switches. they certainly scale better
        return;
    }
    if(theMessage == StopGitBitShitDaemon_Message)
    {
        QTextStream stdOut(stdout);
        stdOut << "GitBitShitDaemon quitting" << endl;
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
}
void GitBitShitDaemon::handleClientDisconnected(GitBitShitDaemonClient *disconnectedClient)
{
    m_CurrentlyConnectedClients.removeOne(disconnectedClient);
    disconnectedClient->deleteLater();
}
