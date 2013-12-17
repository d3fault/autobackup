#ifndef EASYTREE_H
#define EASYTREE_H

#include <QObject>
#include <QList>
#include <QTextStream>
#include <QDateTime>
#include <QFileInfo>
#include <QListIterator>
#include <QDir>
#include <QIODevice>

//Includes only when calculating hash
#include <QCryptographicHash>
#include <QFile>

class EasyTree : public QObject
{
    Q_OBJECT
public:
    enum CalculateMd5Enum { DoCalculateMd5sums, DontCalculateMd5Sums}; //death to boolean method parameters (it isn't just about readability)!
    enum LastModifiedTimestampsFormat { SimplifiedLastModifiedTimestamps, ObsoleteEasyTreeTimestampsWithSizeAndCreationDate };
    explicit EasyTree(QObject *parent = 0);
    ~EasyTree();
private:
    QString m_DirWeAreTreeing;
    int m_DirWeAreTreeingChopLength;
    QDateTime m_CreatedDateTime;
    QDateTime m_ModifiedDateTime;
    QString m_Colon;
    QString m_EscapedColon;
    QIODevice *m_IODevice;
    QTextStream *m_TreeTextStream;
    QDir *m_Dir;
    QFileInfo m_CurrentFileInfo;
    CalculateMd5Enum m_CalculateMd5Sums;
    static const qint64 m_MaxReadSize;
    QList<QString> *m_DirNamesToIgnore;
    QList<QString> *m_FileNamesToIgnore;
    QList<QString> *m_FileNamesEndWithIgnoreList;
    QList<QString> *m_DirNamesEndWithIgnoreList;

    void insertAllFileAndDirInfoIntoTreeTextRecursingIntoDirs(const QString &absoluteDirString);
    inline void addDirEntry();
    inline void addFileEntry();
    bool weDontWantToSkipCurrentFileInfo();
    bool weDontWantToSkipCurrentDirInfo();

    LastModifiedTimestampsFormat m_LastModifiedTimestampsFormat;
signals:
    //void treeTextGenerated(const QString &treeText); //it just writes to the passed in QIODevice instead
public slots:
    void generateTreeText(const QString &absoluteDirString, QIODevice *ioDeviceToWriteTo, CalculateMd5Enum calculateMd5sums, QList<QString> *dirNamesToIgnore, QList<QString> *fileNamesToIgnore, QList<QString> *fileNamesEndWithIgnoreList, QList<QString> *dirNamesEndsWithIgnoreList, LastModifiedTimestampsFormat lastModifiedTimestampsFormat);
};

#endif // EASYTREE_H
