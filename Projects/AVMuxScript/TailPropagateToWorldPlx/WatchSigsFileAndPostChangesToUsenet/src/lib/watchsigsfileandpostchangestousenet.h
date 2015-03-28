#ifndef WATCHSIGSFILEANDPOSTCHANGESTOUSENET_H
#define WATCHSIGSFILEANDPOSTCHANGESTOUSENET_H

#include <QObject>

#include <QDir>
#include <QFileSystemWatcher>
#include <QSet>
#include <QScopedPointer>
#include <QMimeDatabase>
#include <QProcess>

#include "recursivecustomdetachedgpgsignatures.h"

class WatchSigsFileAndPostChangesToUsenet : public QObject
{
    Q_OBJECT
public:
    explicit WatchSigsFileAndPostChangesToUsenet(QObject *parent = 0);
private:
    QFileSystemWatcher *m_SigsFileWatcher;
    RecursiveCustomDetachedSignatures *m_RecursiveCustomDetachedSignatures;
    QSet<RecursiveCustomDetachedSignaturesFileMeta> m_FilesAlreadyPostedOnUsenet;
    QSet<RecursiveCustomDetachedSignaturesFileMeta> m_FilesEnqueuedForPostingToUsenet;
    QScopedPointer<RecursiveCustomDetachedSignaturesFileMeta> m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet;
    QMimeDatabase m_MimeDatabase;
    QProcess *m_PostnewsProcess;

    bool startWatchingSigsFile(const QString &sigsFilePath);
    void readInSigsFileAndPostAllNewEntries(const QString &sigsFilePath);
    void postAnEnqueuedFileIfNotAlreadyPostingOne();
    void postToUsenet(const RecursiveCustomDetachedSignaturesFileMeta &nextFile);
    QByteArray wrap(const QString &toWrap, int wrapAt);
    //bool getMimeFromFileProcess(const QString &filePath, QByteArray *out_Mime);
signals:
    void o(const QString &msg);
    void e(const QString &msg);
    void doneWatchingSigsFileAndPostingChangesToUsenet(bool success);
public slots:
    void startWatchingSigsFileAndPostChangesToUsenet(const QString &sigsFilePathToWatch, const QString &dirCorrespondingToSigsFile, const QString &dataDirForKeepingTrackOfAlreadyPostedFiles);
     void startWatchingSigsFileAndPostChangesToUsenet(const QString &sigsFilePathToWatch, const QDir &dirCorrespondingToSigsFile, const QDir &dataDirForKeepingTrackOfAlreadyPostedFiles);
public slots:
     void quitCleanly();
private slots:
     void handleSigsFileChanged(const QString &sigsFilePath);
     void handlePostnewsProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // WATCHSIGSFILEANDPOSTCHANGESTOUSENET_H
