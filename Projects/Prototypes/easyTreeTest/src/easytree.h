#ifndef EASYTREE_H
#define EASYTREE_H

#include <QObject>
#include <QList>
#include <QTextStream>
#include <QDateTime>
#include <QFileInfo>
#include <QListIterator>
#include <QDir>

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
    QString *m_TreeText;
    QTextStream *m_TreeTextStream;
    QDir *m_Dir;
    QFileInfo m_CurrentFileInfo;
    QList<QString> *m_DirNamesToIgnore;
    QList<QString> *m_FileNamesToIgnore;
    QList<QString> *m_FileNamesEndWithIgnoreList;
    QList<QString> *m_DirNamesEndWithIgnoreList;

    void insertAllFileAndDirInfoIntoTreeTextRecursingIntoDirs(const QString &absoluteDirString);
    inline void addDirEntry();
    inline void addFileEntry();
    bool weDontWantToSkipCurrentFileInfo();
    bool weDontWantToSkipCurrentDirInfo();
signals:
    void treeTextGenerated(const QString &treeText);
public slots:
    void initialize();
    void generateTreeText(const QString &absoluteDirString, QList<QString> *dirNamesToIgnore, QList<QString> *fileNamesToIgnore, QList<QString> *fileNamesEndWithIgnoreList, QList<QString> *dirNamesEndsWithIgnoreList);
};

#endif // EASYTREE_H
