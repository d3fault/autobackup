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
    bool m_CalculateMd5Sums;
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

    bool m_UseObsoleteFormatWithSizeAndCreationDate; //stack explosion
signals:
    //void treeTextGenerated(const QString &treeText); //it just writes to the passed in QIODevice instead
public slots:
    void generateTreeText(const QString &absoluteDirString, QIODevice *ioDeviceToWriteTo, bool calculateMd5Sums, QList<QString> *dirNamesToIgnore, QList<QString> *fileNamesToIgnore, QList<QString> *fileNamesEndWithIgnoreList, QList<QString> *dirNamesEndsWithIgnoreList, bool useObsoleteFormatWithSizeAndCreationDate = true);
};

#endif // EASYTREE_H
