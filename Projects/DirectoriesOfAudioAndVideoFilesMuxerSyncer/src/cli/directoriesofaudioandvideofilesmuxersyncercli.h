#ifndef DIRECTORIESOFAUDIOANDVIDEOFILESMUXERSYNCERCLI_H
#define DIRECTORIESOFAUDIOANDVIDEOFILESMUXERSYNCERCLI_H

#include <QObject>

#include <QTextStream>

class DirectoriesOfAudioAndVideoFilesMuxerSyncerCli : public QObject
{
    Q_OBJECT
public:
    explicit DirectoriesOfAudioAndVideoFilesMuxerSyncerCli(QObject *parent = 0);
private:
    QTextStream m_StdOut;
    QTextStream m_StdErr;

    void cliUsage();
signals:
    void exitRequested(int exitCode);
private slots:
    void handleO(const QString &msg);
    void handleE(const QString &msg);
    void handleDoneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(bool success);
};

#endif // DIRECTORIESOFAUDIOANDVIDEOFILESMUXERSYNCERCLI_H
