#ifndef SIMPLIFIEDLASTMODIFIEDTIMESTAMP_H
#define SIMPLIFIEDLASTMODIFIEDTIMESTAMP_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QBuffer>
#include <QTextStream>

class SimplifiedLastModifiedTimestamp
{
public:
    SimplifiedLastModifiedTimestamp(const QString &colonSeparatedLastModifiedTimestampLine);
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
};

#endif // SIMPLIFIEDLASTMODIFIEDTIMESTAMP_H
