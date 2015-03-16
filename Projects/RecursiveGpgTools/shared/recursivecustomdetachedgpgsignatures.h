#ifndef RECURSIVECUSTOMDETACHEDGPGSIGNATURES_H
#define RECURSIVECUSTOMDETACHEDGPGSIGNATURES_H

#include <QObject>

#include <QTextStream>

#define GPG_DEFAULT_PATH "/usr/bin/gpg" //TODOoptional: custom path as arg

struct RecursiveCustomDetachedSignaturesFileMeta //POD
{
    RecursiveCustomDetachedSignaturesFileMeta() { }
    RecursiveCustomDetachedSignaturesFileMeta(const QString &filePath, const QString &gpgSignature, qint64 unixTimestamp)
        : FilePath(filePath)
        , GpgSignature(gpgSignature)
        , UnixTimestampInSeconds(unixTimestamp)
    { }

    QString FilePath;
    QString GpgSignature;
    qint64 UnixTimestampInSeconds;
};
class RecursiveCustomDetachedSignatures : public QObject
{
    Q_OBJECT
public:
    explicit RecursiveCustomDetachedSignatures(QObject *parent = 0);
    bool readPathAndSignature(QTextStream &customDetachedGpgSignaturesTextStream, QString *out_FilePathToVerify, QString *out_CurrentFileSignature, qint64 *out_CurrentFileUnixTimestamp);
    bool filesystemLastModifiedUnixTimestampAndMetaUnixTimestampsAreIdentical(const QDateTime &filesystemLastModifiedUnixTimestamp, const RecursiveCustomDetachedSignaturesFileMeta &fileMeta);
signals:
    void e(const QString &msg);
};

QTextStream &operator<<(QTextStream &textStream, const RecursiveCustomDetachedSignaturesFileMeta &fileMeta);

#endif // RECURSIVECUSTOMDETACHEDGPGSIGNATURES_H
