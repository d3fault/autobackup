#ifndef QFILESYSTEMWATCHER2_H
#define QFILESYSTEMWATCHER2_H

#include <QFileSystemWatcher>
#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QHashIterator>

struct FileInfoHardCopy
{
    QDateTime CreationDate;
    QDateTime ModifiedDate;
    qint64 Size;
};
typedef QHash<QString,FileInfoHardCopy*> DirStructure;
class QFileSystemWatcher2 : public QFileSystemWatcher
{
    Q_OBJECT
public:
    explicit QFileSystemWatcher2(QObject *parent = 0);

    void addPath(const QString &file);
    void addPaths(const QStringList &files);
    void removePath(const QString &file);
    void removePaths(const QStringList &files);

    QStringList files() const;
    QStringList directories() const;

private:
    void emitSignalsForDirInfoListChanges();
    DirStructure *populateDirStructureFromFileInfoList(QFileInfoList &newFileInfoList);
    DirStructure *m_OldDirStructure;
    DirStructure *m_NewDirStructure;
    void pruneAlreadyEmittedFromBothFileInfoLists(QFileInfo &newList, QFileInfo &oldList, QFileInfo &pruneList);
private slots:
    void figureOutFileChangeFromDirChange(const QString &dirString);
};

#endif // QFILESYSTEMWATCHER2_H
