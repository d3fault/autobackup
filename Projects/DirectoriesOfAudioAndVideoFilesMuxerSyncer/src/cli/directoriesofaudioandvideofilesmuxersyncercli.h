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
    QString m_AudioDelaysOutputFile_OrEmptyStringIfNotInteractivelyCalculatingThem;

    QTextStream m_StdIn;
    QTextStream m_StdOut;
    QTextStream m_StdErr;

    void cliUsage();
signals:
    void userWantsToPreviewThisAudioDelayAgain();
    void userWantsToUseCurrentAudioDelayMs();
    void userWantsToUseCurrentAudioDelayMsForTheRestOfTheVideoFiles();
    void userInputtedNewAudioDelay(qint64 newAudioDelayMs);
    void audioDelaysOutputFilePathChosen(const QString &audioDelaysOutputSaveFilaPath);
    void exitRequested(int exitCode);
private slots:
    void handleO(const QString &msg);
    void handleE(const QString &msg);
    void handletruncatedPreviewedVideoFileReadyForAudioDelayUserInput(qint64 currentAudioDelayMs);
    void handleAudioDelaysFileCalculatedSoRespondWithWhereToSaveIt();
    void handleDoneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(bool success);
};

#endif // DIRECTORIESOFAUDIOANDVIDEOFILESMUXERSYNCERCLI_H
