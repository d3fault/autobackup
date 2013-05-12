#ifndef EASYTREEHASHITEM_H
#define EASYTREEHASHITEM_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QByteArray>
#include <QBuffer>
#include <QTextStream>

class EasyTreeHashItem : public QObject
{
    Q_OBJECT
public:
    EasyTreeHashItem(QObject *parent = 0);
    static EasyTreeHashItem *newEasyTreeHashItemFromLineOfText(const QString &lineOfText, QObject *parent = 0); //works with files, directories, and with or without hashes
    QString relativeFilePath();
    bool isDirectory();
    qint64 fileSize();
    QDateTime creationDateTime();
    QDateTime lastModifiedDateTime();
    bool hasHash();
    QByteArray hash();
private:
    QString m_RelativeFilePath;
    bool m_IsDirectory;
    qint64 m_FileSize;
    QDateTime m_CreationDateTime;
    QDateTime m_LastModifiedDateTime;
    bool m_HasHash;
    QByteArray m_Hash;

    void setRelativeFilePath(const QString &relativeFilePath);
    void setIsDirectory(bool isDirectory);
    void setFileSize(qint64 fileSize);
    void setCreationDateTime(const QDateTime &creationDateTime);
    void setLastModifiedDateTime(const QDateTime &lastModifiedDateTime);
    void setHasHash(bool hasHash);
    void setHash(const QByteArray &hash);


    static const QString m_Colon;
    static QString m_TempStringToInsertForEscapedColons; //even though it's a member (optimization) it is still generated on each invocation (whenever an escaped colon is seen)
    static const QString m_ColonEscaped;

    static QString generateStringNotInString_ButMakeSureTheOutputDoesntContainAcolon(const QString &inputStringToCheckGeneratedOutputAgainst);
};

#endif // EASYTREEHASHITEM_H
