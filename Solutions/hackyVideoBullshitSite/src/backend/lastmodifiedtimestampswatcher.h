#ifndef LASTMODIFIEDTIMESTAMPSWATCHER_H
#define LASTMODIFIEDTIMESTAMPSWATCHER_H

#include <QObject>
#include <QStringList>
#include <QHash>
#include <QAtomicPointer>
#include <QQueue>
#include <QListIterator>

#include "../lastmodifiedtimestampsandpaths.h"
#include "lastmodifiedtimestampssorter.h"

class QTimer;
class QFileSystemWatcher;

class LastModifiedTimestampsWatcher : public QObject
{
    Q_OBJECT
public:
    explicit LastModifiedTimestampsWatcher(QObject *parent = 0);
    QAtomicPointer<LastModifiedTimestampsAndPaths> *getTimestampsAndPathsAtomicPointer();
    ~LastModifiedTimestampsWatcher();
private:
    QFileSystemWatcher *m_LastModifiedTimestampsFilesWatcher;
    QString m_AbsolutePathToBaseViewDirSoTheFoldersInBetweenThereAndLastModifiedTimestampsFilesCanBeUsedAsPrefixInCombinedSortedList;
    QHash<QString, SortedMapOfListsOfPathsPointerType*> m_LastModifiedTimestampsFilesAndPreCombinedSemiSortedTimestampsAndPathsMaps;
    QAtomicPointer<LastModifiedTimestampsAndPaths> m_CurrentTimestampsAndPathsAtomicPointer;
    QTimer *m_DeleteInFiveMinsTimer;
    QQueue<LastModifiedTimestampsAndPaths*> *m_TimestampsAndPathsQueuedForDelete;

    bool addAndReadLastModifiedTimestampsFile(const QString &lastModifiedTimestampsFile);
    QString getPathPrefixForThisLastModifiedTimestampsFile(const QString &lastModifiedTimestampsFile);
    void combineAndPublishLastModifiedTimestampsFiles();
    void deleteOneTimestampAndPathQueuedForDelete();

    inline QString appendSlashIfNeeded(const QString &inputString) { return inputString.endsWith("/") ? inputString : (inputString + "/"); }
signals:
    void e(const QString &);
    void startedWatchingLastModifiedTimestampsFile();
public slots:
    void startWatchingLastModifiedTimestampsFiles(const QString &absolutePathToBaseViewDirSoTheFoldersInBetweenThereAndLastModifiedTimestampsFilesCanBeUsedAsPrefixInCombinedSortedList, const QStringList &lastModifiedTimestampsFiles);
private slots:    
    void handleLastModifiedTimestampsChanged(const QString &lastModifiedTimestampsFileThatChanged);
    void handleDeleteInFiveMinsTimerTimedOut();
};

#endif // LASTMODIFIEDTIMESTAMPSWATCHER_H
