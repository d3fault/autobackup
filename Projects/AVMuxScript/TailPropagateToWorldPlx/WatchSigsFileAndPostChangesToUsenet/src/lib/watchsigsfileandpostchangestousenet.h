#ifndef WATCHSIGSFILEANDPOSTCHANGESTOUSENET_H
#define WATCHSIGSFILEANDPOSTCHANGESTOUSENET_H

#include <QObject>

#include <QDir>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QSet>
#include <QScopedPointer>
#include <QMimeDatabase>
#include <QProcess>
#include <QTemporaryDir>
#include <QDirIterator>
#include <QFileInfo>

#include "recursivecustomdetachedgpgsignatures.h"

struct UsenetPostDetails //POD
{
    QByteArray Boundary;
    QByteArray MessageId;
    QByteArray Mime;
    QByteArray FileNameOnly;
    QByteArray ContentMd5Base64;
    QByteArray Body;
    bool AttachGpgSig;
    QByteArray GpgSigMaybe;
    QString AbsoluteFilePath;
};
struct RecursiveCustomDetachedSignaturesFileMetaAndListOfMessageIDs //POD
{
    RecursiveCustomDetachedSignaturesFileMetaAndListOfMessageIDs(const RecursiveCustomDetachedSignaturesFileMeta &fileMeta)
        : FileMeta(fileMeta)
    { }
    RecursiveCustomDetachedSignaturesFileMeta FileMeta;
    QStringList SuccessfullyPostedMessageIDs;
    UsenetPostDetails PostInProgressDetails;
};

class QTimer;

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
    QDir m_DirCorrespondingToSigsFile;
    QScopedPointer<QSettings> m_AlreadyPostedFiles;
    RecursiveCustomDetachedSignatures *m_RecursiveCustomDetachedSignatures;
    QHash<QString /* relative file path*/, RecursiveCustomDetachedSignaturesFileMeta> m_FilesEnqueuedForPostingToUsenet;
    QScopedPointer<RecursiveCustomDetachedSignaturesFileMetaAndListOfMessageIDs> m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet;
    QScopedPointer<QTemporaryDir> m_FileCurrentlyBeingPostedToUsenetVolumesTempDir_OrNullIfFileCurrentlyBeingPostedDidntNeedToBeSplit;
    QScopedPointer<QDirIterator> m_FileCurrentlyBeingPostedToUsenetVolumesTempDirIterator;
    QMimeDatabase m_MimeDatabase;
    QProcess *m_PostnewsProcess;
    QTimer *m_RetryWithExponentialBackoffTimer;
    int m_NumFailedPostAttemptsInArow;
    bool m_CleanQuitRequested;

    bool startWatchingSigsFileIfNotAlreadyWatching(const QString &sigsFilePath);
    bool ensureExistsAndIsExecutable(const QString &binaryToVerify);
    void readInSigsFileAndPostAllNewEntries(const QString &sigsFilePath);
    void postAnEnqueuedFileIfNotAlreadyPostingOne_OrQuitIfCleanQuitRequested();
    void postToUsenet(const RecursiveCustomDetachedSignaturesFileMeta &nextFile);
    void postNextVolumePartInDir_OrContinueOntoNextFullFileIfAllPartsOfCurrentFileHaveBeenPosted();
    void beginPostingToUsenetAfterBase64encoding(const QFileInfo &fileInfo, const QString &gpgSignature_OrEmptyStringIfNotToAttachOne = QString()/* when we split a file into parts, we put the sig _IN_ the archive, so we don't need to attach the sig [to each part] */, const QString &mimeType_OrEmptyStringIfToFigureItOut = QString());
    QByteArray wrap(const QByteArray &toWrap, int wrapAt);
    QByteArray generateRandomAlphanumericBytes(int maxBytesToGenerate, int minBytesToGenerate = 10);
    void handleFullFilePostedToUsenet();
    bool checkAlreadyPostedFilesForError();

    inline QString appendSlashIfNeeded(const QString &inputString) { if(inputString.endsWith("/")) return inputString; return inputString + "/"; }
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
     void generateMessageIdAndPostToUsenet();
     void handlePostnewsProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // WATCHSIGSFILEANDPOSTCHANGESTOUSENET_H
