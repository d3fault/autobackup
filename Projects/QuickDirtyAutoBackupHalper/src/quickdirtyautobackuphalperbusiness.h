#ifndef QUICKDIRTYAUTOBACKUPHALPERBUSINESS_H
#define QUICKDIRTYAUTOBACKUPHALPERBUSINESS_H

#include <QObject>
#include <QStringList>
#include <QListIterator>
#include <QList>
#include <QPair>
#include <QSettings>
#include <QFileInfo>
#include <QProcess>
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QTimer>

#include "easytree.h"

#define TC_CONTAINERS_AND_PW_FILES_ARRAY_STRING "tcContainersAndPwFilesArray"
#define TC_CONTAINERS_PATH_STRING_KEY "tcContainerPath"
#define TC_PASSWORD_FILE_PATH_STRING_KEY "tcPasswordPath"

#define DEFAULT_SHUTDOWN_SECONDS_COUNTDOWN_AMOUNT 3

//TODOoptional: detect when i accidentally overwrite a [text] file. how do i differentiate it from a typical [source] rewrite (git detects that for me btw ;-)) <-- solution: ignore cpp/h/pro rewrites, flag/STOP/ask-me-if-i-am-sure when it's a .txt file? kinda hacky but would work. could even automatically fix it after showing a diff between the files :) ok getting ahead of myself here...

//TODOoptional: an opt-in checkbox 'init shared bare repo in subdir (creating that too if needed)'. two checks areperformed to see 'if needed': if the sub-dir doesn't exist, OR if some identifying file in there doesn't exist GIT_IGNORE or GIT_LOG or some random file i've seen before in the bare git shared repos. command for this btw is: git init --bare --shared=true
//^the checkbox has to be checked first of all. we error out otherwise (i guess that means we do the check for the file(s) either way? sounds reasonable TODOreq)

//TODOoptional: instead of passing the gitignore patterns via command line as i'm planning, i can check to see if the .git/info/exclude file is in it's default state. if it is, i can overwrite it. this way i can still use git from the command line without it getting files my gui has been telling git to ignore this whole time

class QuickDirtyAutoBackupHalperBusiness : public QObject
{
    Q_OBJECT
public:
    explicit QuickDirtyAutoBackupHalperBusiness(QObject *parent = 0);
    ~QuickDirtyAutoBackupHalperBusiness();
private:
    EasyTree *m_EasyTree;
    struct TruecryptVerboseListMountPointDetailsObject
    {
        //Slot, //: 22
        QString TcContainerPath; /*: /home/d3fault/Temp/testUserTcContainer*/
        //VirtualDevice, //: /dev/mapper/truecrypt22
        QString TcMountPath; /*/: /media/truecrypt22*/
        //Size, //: 24.8 MB
        //Type, //: Normal
        bool TcMountedAsReadOnly; //: No
        //HiddenVolumeProtected, //: No
        //EncryptionAlgorithm, //: AES
        //Primary Key Size: 256 bits
        //Secondary Key Size (XTS Mode): 256 bits
        //Block Size: 128 bits
        //Mode of Operation: XTS
        //PKCS-5 PRF: HMAC-RIPEMD-160
        //Volume Format Version: 2
        //Embedded Backup Header: Yes

        TruecryptVerboseListMountPointDetailsObject()
        {
            //this->TcMounted = false;
            this->TcContainerPath.clear();
            this->TcMountPath.clear();
            this->TcMountedAsReadOnly = true;
        }
    };
    inline void readSettingsAndProbeMountStatus();
    QList<TruecryptVerboseListMountPointDetailsObject*> *m_ListOfMountedContainers;
    void scanForMountedRelevantContainers(QList<QPair<QString,QString> > *listOfContainers);
    QList<QPair<QString,QString> > *m_InternalTcContainerAndPasswordPathsList;
    bool allContainerAndPwFilePathsAreValid(QList<QPair<QString,QString> > *pathsToCheck);
    bool existsAndIsNotDirNorSymLink(const QString &pathToCheck);
    bool probeMountStatusOfInternalListAndEmitIfEitherAllMountedOrAllDismounted();
    bool noneRelevantAreAlreadyMounted(QList<QPair<QString,QString> > *listOfRelevant);
    bool noDuplicatesExistInList(QList<QPair<QString,QString> > *list);
    bool verifyAllAreMounted(QList<QPair<QString,QString> > *listToVerify);
    void dismountEverythingRegardlessOfItBeingMineOrNot();
    void start3secondShutdownTimerDispatchingUpdatesAtEachSecondInterval();
    void actuallyCommitToListOfMountedContainers(const QString &commitMsg, const QString &workingDirString, const QString &subDirOnMountPointToBareGitRepo, const QString &dirStructureFileNameString, QStringList *filenameIgnoreList, QStringList *filenameEndsWithIgnoreList, QStringList *dirnameIgnoreList, QStringList *dirnameEndsWithIgnoreList, bool pushEvenWhenNothingToCommit);
    void actualShutdown();
    bool m_StillUsingOurOwnAllocatedInternalList;
    QFileInfo m_FileInfo;
    QString m_PathToTruecryptBinary;
    QString m_PathToGitBinary;
    QProcess *m_Process;
    QTimer *m_ShutdownTimer;
    QStringList m_TypicalTruecryptArgs;
    QStringList *m_FileIgnoreListForTreeOnlyWithTheDirStructureFileIncluded;
    quint16 m_SecondsLeftUntilShutdown;
    bool m_ShuttingDown;
    bool m_MountedAsReadOnlyFlagForDismount;
signals:
    void d(const QString &msg);
    void brokenStateDetected();
    void readSettingsAndProbed(bool allMountedTrueOrAllDismountedFalse, bool mountedAsReadOnly, QList<QPair<QString,QString> > *tcContainerAndPasswordPaths);
    void mounted();
    void dismounted();
    void committed();
public slots:
    void start();
    void stop();
    void pushToGitIgnore(const QString&workingDir, QStringList *filenamesIgnoreList, QStringList *filenamesEndsWithIgnoreList, QStringList *dirnamesIgnoreList, QStringList *dirnamesEndsWithIgnoreList);
    void persist(QList<QPair<QString,QString> > *tcContainerAndPasswordPathsToPersist);
    void mount(QList<QPair<QString,QString> > *tcContainerAndPasswordPathsToMount, bool mountAsReadOnly);
    void dismount();
    void commit(const QString &commitMsg, QList<QPair<QString,QString> > *tcContainerAndPasswordPathsFromGui, const QString &workingDirString, const QString &subDirOnMountPointToBareGitRepo, const QString &dirStructureFileNameString, QStringList *filenameIgnoreList, QStringList *filenameEndsWithIgnoreList, QStringList *dirnameIgnoreList, QStringList *dirnameEndsWithIgnoreList, bool pushEvenWhenNothingToCommit);
    void shutdown();
    void cancelShutdown();
private slots:
    void handleShutdownTimerTimedOut();
};

#endif // QUICKDIRTYAUTOBACKUPHALPERBUSINESS_H
