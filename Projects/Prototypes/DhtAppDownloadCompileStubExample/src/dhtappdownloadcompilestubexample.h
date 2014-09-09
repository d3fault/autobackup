#ifndef DHTAPPDOWNLOADCOMPILESTUBEXAMPLE_H
#define DHTAPPDOWNLOADCOMPILESTUBEXAMPLE_H

#include <QObject>

class Dht;

class DhtAppDownloadCompileStubExample : public QObject
{
    Q_OBJECT
public:
    explicit DhtAppDownloadCompileStubExample(QObject *parent = 0);
    virtual ~DhtAppDownloadCompileStubExample();
private:
    Dht *m_Dht;

    QString binaryToFilesystem(const QByteArray &binary);
signals:
    void e(const QString &msg);
public slots:
    void stubFirstRun();
    QByteArray dhtDownload(QList<QString> dhtSeedIpAddresses, const QString &sha1ofA7zThatIsKnownToBeOnTheP2pNetwork);
    QString compile(const QString &dirToRunQmakeMakeIn_ItWasJustUn7zadAfterDlingFromP2p);
};

#endif // DHTAPPDOWNLOADCOMPILESTUBEXAMPLE_H
