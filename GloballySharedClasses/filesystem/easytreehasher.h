#ifndef EASYTREEHASHER_H
#define EASYTREEHASHER_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QIODevice>
#include <QTextStream>
#include <QCryptographicHash>
#include <QListIterator>
#include <QDateTime>

class EasyTreeHasher
{
    Q_OBJECT
public:
    explicit EasyTreeHasher(QObject *parent = 0);
    void recursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay(const QDir &sourceDir, const QDir &emptyDestinationDir, const QIODevice *easyTreeHashOutputIODevice, QCryptographicHash::Algorithm algorithm);
    ~EasyTreeHasher();
private:
    static const qint64 m_MaxReadSize;
    QString m_DirSeparator;
    QString m_Colon;
    QString m_EscapedColon;

    QDir::Filters m_ConservativeFiltersForSeeingIfThereAreFilesToCopy;
    QDir::SortFlags m_SortFlags;

    QCryptographicHash *m_Hasher;
    QByteArray m_HashResult;
    QTextStream m_EasyTreeHashTextStream;

    QString getCurrentSourceFileInfo_RelativePath();
    QString getCurrentSourceFileInfo_FilenameOrDirnameOnly();
    QString getCurrentSourceFileInfoPath_ColonEscapedAndRelative();
    int m_SourceDirectoryAbsolutePathLength;
    QString m_CurrentFilenameOrDirnameOnly; //dir name or file name
    QFileInfo m_CurrentSourceFileInfo;
    QFile m_SourceFile2CopyTreeAndHash; //Treeing is done by the QFileInfo though...
    QFile m_DestinationFile2Write;

    //The Beef:
    void copyEachOfTheseFilesToTheDestinationAndRecurseIntoDirsDoingTheSameWhileMkDiringIntoDestinationOhAndAlsoWritingEverythingToEasyTreeHashOutputIODeviceRofl(const QFileInfoList &filesAndFoldersInCurrentDirToCopyAndTreeAndHash, const QDir &destAlreadyMkdirDAndCDdInto); //that QDir might not be able to be const? I suck at const'ness so idfk

    void addDirectoryEntryToEasyTreeHashOutput();
    void copyAndHashSimultaneously(const QDir &destDir);
    void addFileEntryToEasyTreeHashOutput();
signals:
    void d(const QString &); //use as recoverableError.. just follow the message with "return;"
    void nonRecoverableError(const QString &); //does more or less the same as recoverableError, but disables GUI too
};

#endif // EASYTREEHASHER_H
