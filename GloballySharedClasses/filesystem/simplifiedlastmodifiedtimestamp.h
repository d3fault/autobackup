#ifndef SIMPLIFIEDLASTMODIFIEDTIMESTAMP_H
#define SIMPLIFIEDLASTMODIFIEDTIMESTAMP_H

#include <QString>
#include <QStringList>
#include <QDateTime>

class SimplifiedLastModifiedTimestamp
{
public:
    enum IsDirectoryDetectionEnum
    {
        DirectoryIfEndingInSlashAutoDetection,
        IsDirectory,
        IsNotDirectory
    };

    //parsing constructor
    SimplifiedLastModifiedTimestamp(const QString &colonSeparatedLastModifiedTimestampLine);
    //serializing constructor
    SimplifiedLastModifiedTimestamp(const QString &filePath, const QDateTime &lastModifiedTimestamp, IsDirectoryDetectionEnum isDirectoryDetection = DirectoryIfEndingInSlashAutoDetection);
    const QString &filePath();
    bool isDirectory();
    const QDateTime &lastModifiedTimestamp();
    QString toColonSeparatedLineOfText();
private:
    QString m_FilePath;
    bool m_IsDirectory;
    QDateTime m_LastModifiedTimestamp;

    static const QString m_Colon;
    static const QString m_ColonEscaped;
    static const QString m_IllegalFilePath;

    inline void detectIfFilePathIsDirectory() { m_IsDirectory = m_FilePath.endsWith("/") ? true : false; }
    inline QString appendSlashIfNeeded(const QString &inputString) { return inputString.endsWith("/") ? inputString : (inputString + "/"); }
};

#endif // SIMPLIFIEDLASTMODIFIEDTIMESTAMP_H
