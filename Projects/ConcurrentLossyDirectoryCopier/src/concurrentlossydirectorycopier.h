#ifndef CONCURRENTLOSSYDIRECTORYCOPIER_H
#define CONCURRENTLOSSYDIRECTORYCOPIER_H

#include <QObject>
#include <QStringList>
#include <QPair>
#include <QFile>
#include <QDebug>
#include <QThread>
#include <QProcess> //threads managing processes, fuck yeh~
#include <QProcessEnvironment>
#include <qtconcurrentmap.h>
#include <QTextStream>
#include <QList>
#include <QDir>
#include <QFileInfoList>
#include <QFuture>
#include <QFutureWatcher>

#include "copyoperation.h"

class ConcurrentLossyDirectoryCopier : public QObject
{
    Q_OBJECT
public:
    explicit ConcurrentLossyDirectoryCopier(QObject *parent = 0);
    static QPair<int, int> calculateOrGuessNewAudioAndVideoBitrates(const QString &videoFilePath, double newBitratePercentTarget = 0.75);
    static int parsedBitrateCheckForKbsStringAndGreaterThanZero_or_NegativeTwo(const QString &bitrateStringMaybe);
    static int guessMissingKiloBitRateBasedOn_KnownKiloBitRate_Duration_and_SizeBytes(int knownKiloBitRate, int durationInSeconds, int sizeBytes);
private:
    QList<QString> m_VideoExtensions;
    QList<QString> m_SpecialCaseRawAudioExtensions;
    QList<QString> m_AudioExtensions;
    QList<QString> m_ImageExtensions;

    QDir::Filters m_DirFilter;
    QString m_AbsoluteDestinationFolderPathWithSlashAppended;
    int m_AbsoluteSourceFolderPathWithSlashAppended_LENGTH;

    QList<CopyOperation> m_ListOfCopyOperationsToDoConcurrently;
    bool recursivelyAddFileEntriesToListAndMakeDestinationDirectories(QFileInfoList &rootFileInfoList);
    QFuture<void> m_FutureForMap;
    QFutureWatcher<void> m_FutureForMapWatcher;
signals:
    void progressMinsAndMaxesCalculated(int progressMinimum, int progressMaximum);
    void progressValueUpdated(int newProgressValue);
    void d(const QString &);
public slots:
    void lossilyCopyDirectoryUsingAllAvailableCores(const QString &sourceDirToLossilyCopyString, const QString &destinationDirToLossilyCopyToString);
    void cancelAfterAllCurrentlyEncodingVideosFinish();
private slots:
    void handleFutureForMapWatcherFinished();
    void handleFutureForMapCancelled();
};

#endif // CONCURRENTLOSSYDIRECTORYCOPIER_H
