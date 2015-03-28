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
    QString m_UsenetAuthUsername;
    QString m_UsenetAuthPassword;
    QString m_PortString;
    QString m_UsenetServer;
    QFileSystemWatcher *m_SigsFileWatcher;
    QString m_AlreadyPostedFilesAllSigsIshFilePath;
    RecursiveCustomDetachedSignatures *m_RecursiveCustomDetachedSignatures;
    QSet<RecursiveCustomDetachedSignaturesFileMeta> m_FilesAlreadyPostedOnUsenet;
    QSet<RecursiveCustomDetachedSignaturesFileMeta> m_FilesEnqueuedForPostingToUsenet;
    QScopedPointer<RecursiveCustomDetachedSignaturesFileMeta> m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet;
    QMimeDatabase m_MimeDatabase;
    QProcess *m_PostnewsProcess;
    bool m_CleanQuitRequested;

    bool startWatchingSigsFile(const QString &sigsFilePath);
    bool readInAlreadyPostedFilesSoWeDontDoublePost();
    void readInSigsFileAndPostAllNewEntries(const QString &sigsFilePath);
    void postAnEnqueuedFileIfNotAlreadyPostingOne_OrQuitIfCleanQuitRequested();
    void postToUsenet(const RecursiveCustomDetachedSignaturesFileMeta &nextFile);
    QByteArray wrap(const QString &toWrap, int wrapAt);
    //bool getMimeFromFileProcess(const QString &filePath, QByteArray *out_Mime);
    bool rememberFilesAlreadyPostedOnUsenetSoWeKnowWhereToResumeNextTime();
signals:
    void o(const QString &msg);
    void e(const QString &msg);
    void doneWatchingSigsFileAndPostingChangesToUsenet(bool success);
public slots:
    void startWatchingSigsFileAndPostChangesToUsenet(const QString &sigsFilePathToWatch, const QString &dirCorrespondingToSigsFile/*, const QString &dataDirForKeepingTrackOfAlreadyPostedFiles*/, const QString &authUser, const QString &authPass, const QString &portString, const QString &server);
     void startWatchingSigsFileAndPostChangesToUsenet(const QString &sigsFilePathToWatch, const QDir &dirCorrespondingToSigsFile/*, const QDir &dataDirForKeepingTrackOfAlreadyPostedFiles*/, const QString &authUser, const QString &authPass, const QString &portString, const QString &server);
public slots:
     void quitCleanly();
private slots:
     void handleSigsFileChanged(const QString &sigsFilePath);
     void handlePostnewsProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // WATCHSIGSFILEANDPOSTCHANGESTOUSENET_H
