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
    static EasyTreeHashItem *newEasyTreeHashItemFromLineOfText(const QString &lineOfText, bool lineOfTextSuppliesAbsoluteReplacementFilePath = false, QObject *parent = 0);
    static EasyTreeHashItem *newEasyTreeHashItemFromLineOfText(const QString &lineOfText, bool lineOfTextSuppliesAbsoluteReplacementFilePath = false, bool lineHasCreationDate = true, QObject *parent = 0); //works with files, directories, and with or without hashes -- and a mode switch bool to process replacement formatted entries (the replacement is the first part of the entry)
    QString relativeFilePath();
    bool isDirectory();
    qint64 fileSize();
    bool hasCreationDateTime(); //this is pretty worthless since we still have to pass in the bool to our parser :-P
    QDateTime creationDateTime();
    QDateTime lastModifiedDateTime();
    bool hasHash();
    QByteArray hash();
    QString toColonSeparatedLineOfText();

    QString absoluteReplacementFilePath();

    void setRelativeFilePath(const QString &relativeFilePath);
    void setIsDirectory(bool isDirectory);
    void setFileSize(qint64 fileSize);
    void setHasCreationDateTime(bool hasCreationDateTime);
    void setCreationDateTime(const QDateTime &creationDateTime);
    void setLastModifiedDateTime(const QDateTime &lastModifiedDateTime);
    void setHasHash(bool hasHash);
    void setHash(const QByteArray &hash);

    void setAbsoluteReplacementFilePath(const QString &absoluteReplacementFilePath);
private:
    QString m_RelativeFilePath;
    bool m_IsDirectory;
    qint64 m_FileSize;
    bool m_HasCreationDateTime;
    QDateTime m_CreationDateTime;
    QDateTime m_LastModifiedDateTime;
    bool m_HasHash;
    QByteArray m_Hash;

    QString m_AbsoluteReplacementFilePath;

    static const QString m_Colon;
    static QString m_TempStringToInsertForEscapedColons; //even though it's a member (optimization) it is still generated on each invocation (whenever an escaped colon is seen)
    static const QString m_ColonEscaped;

    static QString generateStringNotInString_ButMakeSureTheOutputDoesntContainAcolon(const QString &inputStringToCheckGeneratedOutputAgainst);
};

#endif // EASYTREEHASHITEM_H
