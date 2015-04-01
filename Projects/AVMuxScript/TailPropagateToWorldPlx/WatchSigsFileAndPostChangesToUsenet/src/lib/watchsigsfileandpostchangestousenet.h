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

//struct WatchSigsFileAndPostChangesToUsenet_CurrentlyBeingUploadedType //lol for lack of a better name
struct RecursiveCustomDetachedSignaturesFileMetaAndListOfMessageIDs //POD
{
    RecursiveCustomDetachedSignaturesFileMetaAndListOfMessageIDs(const RecursiveCustomDetachedSignaturesFileMeta &fileMeta)
        : FileMeta(fileMeta)
    { }
    RecursiveCustomDetachedSignaturesFileMeta FileMeta;
    QStringList MessageIDs;
};

Q_DECLARE_METATYPE(QList<QByteArray>)

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
    //QHash<QString /* relative file path*/, QStringList /* MessageIDs */> m_FilesAlreadyPostedOnUsenet_AndTheirMessageIDs;
    QHash<QString /* relative file path*/, RecursiveCustomDetachedSignaturesFileMeta> m_FilesEnqueuedForPostingToUsenet;
    QScopedPointer<RecursiveCustomDetachedSignaturesFileMetaAndListOfMessageIDs> m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet;
    QScopedPointer<QTemporaryDir> m_FileCurrentlyBeingPostedToUsenetVolumesTempDir_OrNullIfFileCurrentlyBeingPostedDidntNeedToBeSplit;
    QScopedPointer<QDirIterator> m_FileCurrentlyBeingPostedToUsenetVolumesTempDirIterator;
    QMimeDatabase m_MimeDatabase;
    QByteArray m_MessageIdCurrentlyPostingWith;
    QProcess *m_PostnewsProcess;
    bool m_CleanQuitRequested;

    bool startWatchingSigsFile(const QString &sigsFilePath);
    //bool readInAlreadyPostedFilesSoWeDontDoublePost();
    void readInSigsFileAndPostAllNewEntries(const QString &sigsFilePath);
    void postAnEnqueuedFileIfNotAlreadyPostingOne_OrQuitIfCleanQuitRequested();
    void postToUsenet(const RecursiveCustomDetachedSignaturesFileMeta &nextFile);
    void postNextVolumePartInDir_OrContinueOntoNextFullFileIfAllPartsOfCurrentFileHaveBeenPosted();
    void beginPostingToUsenetAfterBase64encoding(const QFileInfo &fileInfo, const QString &gpgSignature_OrEmptyStringIfNotToAttachOne = QString()/* when we split a file into parts, we put the sig _IN_ the archive, so we don't need to attach the sig [to each part] */, const QString &mimeType_OrEmptyStringIfToFigureItOut = QString());
    QByteArray wrap(const QString &toWrap, int wrapAt);
    QByteArray generateRandomAlphanumericBytes(int numBytesToGenerate);
    void handleFullFilePostedToUsenet();
    //bool rememberFilesAlreadyPostedOnUsenetSoWeKnowWhereToResumeNextTime();
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
     void handlePostnewsProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // WATCHSIGSFILEANDPOSTCHANGESTOUSENET_H
