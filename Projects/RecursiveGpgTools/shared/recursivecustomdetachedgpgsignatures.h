#ifndef RECURSIVECUSTOMDETACHEDGPGSIGNATURES_H
#define RECURSIVECUSTOMDETACHEDGPGSIGNATURES_H

#include <QObject>

#include <QHash>
#include <QTextStream>

#define GPG_DEFAULT_PATH "gpg" //TODOoptional: custom path as arg
#define RecursiveGpgTools_EXCLUDE_ARG "--exclude" //This is more appropriately in a "clishared" file

struct RecursiveCustomDetachedSignaturesFileMeta //POD
{
    RecursiveCustomDetachedSignaturesFileMeta() { }
    RecursiveCustomDetachedSignaturesFileMeta(const RecursiveCustomDetachedSignaturesFileMeta &other)
        : FilePath(other.FilePath)
        , GpgSignature(other.GpgSignature)
        , UnixTimestampInSeconds(other.UnixTimestampInSeconds)
    { }
    RecursiveCustomDetachedSignaturesFileMeta(const QString &filePath, const QString &gpgSignature, qint64 unixTimestampInSeconds)
        : FilePath(filePath)
        , GpgSignature(gpgSignature)
        , UnixTimestampInSeconds(unixTimestampInSeconds)
    { }

    QString FilePath;
    QString GpgSignature;
    qint64 UnixTimestampInSeconds;

    bool operator==(const RecursiveCustomDetachedSignaturesFileMeta &other) const;
};
inline uint qHash(const RecursiveCustomDetachedSignaturesFileMeta &key, uint seed)
{
    return qHash(key.FilePath, seed) ^ qHash(key.GpgSignature, seed) ^ key.UnixTimestampInSeconds;
}
class RecursiveCustomDetachedSignatures : public QObject
{
    Q_OBJECT
public:
    explicit RecursiveCustomDetachedSignatures(QObject *parent = 0);
    bool readInAllSigsFromSigFile(QIODevice *sigsFile, QHash<QString /*file path*/, RecursiveCustomDetachedSignaturesFileMeta /*file meta*/> *sigsFromSigFile);
    bool readPathAndSignature(QTextStream &customDetachedGpgSignaturesTextStream, QString *out_FilePathToVerify, QString *out_CurrentFileSignature, qint64 *out_CurrentFileUnixTimestamp);
    bool filesystemLastModifiedUnixTimestampAndMetaUnixTimestampsAreIdentical(qint64 filesystemLastModifiedUnixTimestampInSeconds, const RecursiveCustomDetachedSignaturesFileMeta &fileMeta);
signals:
    void e(const QString &msg);
};

QTextStream &operator<<(QTextStream &textStream, const RecursiveCustomDetachedSignaturesFileMeta &fileMeta);

#endif // RECURSIVECUSTOMDETACHEDGPGSIGNATURES_H
