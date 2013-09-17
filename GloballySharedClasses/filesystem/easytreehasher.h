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

#include "easytreehashitem.h"

class EasyTreeHasher : public QObject
{
    Q_OBJECT
public:
    explicit EasyTreeHasher(QObject *parent = 0);
    void recursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay(QDir &sourceDir, QDir &emptyDestinationDir, QIODevice *easyTreeHashOutputIODevice, QCryptographicHash::Algorithm algorithm);
    static EasyTreeHashItem* copyAndHashSimultaneously(const QFileInfo &sourceFileInfoWithAbsolutePath, const QDir &destDir, QCryptographicHash::Algorithm cryptographicHashAlgorithm, int absoluteSourcePathLengthWithTrailingSlash);
private:
    static const qint64 m_MaxReadSize;
    static const QString m_DirSeparator;
    QString m_Colon;
    QString m_EscapedColon;

    QDir::Filters m_ConservativeFiltersForSeeingIfThereAreFilesToCopy;
    QDir::SortFlags m_SortFlags;

    QCryptographicHash::Algorithm m_CryptographicHashAlgorithm;

    QTextStream m_EasyTreeHashTextStream;

    QString getCurrentSourceFileInfo_RelativePath();
    QString getCurrentSourceFileInfo_FilenameOrDirnameOnly();
    static QString getFilenameOrDirnameOnly(QString filepathOrDirPath);
    QString getCurrentSourceFileInfoPath_ColonEscapedAndRelative();
    int m_SourceDirectoryAbsolutePathLength;
    QString m_CurrentFilenameOrDirnameOnly; //dir name or file name
    QFileInfo m_CurrentSourceFileInfo;

    //The Beef:
    void copyEachOfTheseFilesToTheDestinationAndRecurseIntoDirsDoingTheSameWhileMkDiringIntoDestinationOhAndAlsoWritingEverythingToEasyTreeHashOutputIODeviceRofl(const QFileInfoList &filesAndFoldersInCurrentDirToCopyAndTreeAndHash, QDir &destAlreadyMkdirDAndCDdInto); //that QDir might not be able to be const? I suck at const'ness so idfk

    void addDirectoryEntryToEasyTreeHashOutput();
signals:
    void d(const QString &); //use as recoverableError.. just follow the message with "return;"
    void e(const QString &); //I don't have unrecoverable errors implemented yet (erroring out of recursion is a bitch heh)... but at the very least I want to be able to differentiate between actual errors and simple debug messages. If I receive ANY error messages, I know the output is unreliable
    void nonRecoverableError(const QString &); //does more or less the same as recoverableError, but disables GUI too
};

#endif // EASYTREEHASHER_H
