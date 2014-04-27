#ifndef LASTMODIFIEDTIMESTAMPSWATCHER_H
#define LASTMODIFIEDTIMESTAMPSWATCHER_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QAtomicPointer>
#include <QQueue>
#include <QListIterator>

#include "../lastmodifiedtimestampsandpaths.h"

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
    QFileSystemWatcher *m_LastModifiedTimestampsFileWatcher;
    QString m_LastModifiedTimestampsFile;
    QAtomicPointer<LastModifiedTimestampsAndPaths> m_CurrentTimestampsAndPathsAtomicPointer;
    QTimer *m_DeleteInFiveMinsTimer;
    QQueue<LastModifiedTimestampsAndPaths*> *m_TimestampsAndPathsQueuedForDelete;

    void deleteOneTimestampAndPathQueuedForDelete();
signals:
    void e(const QString &);
public slots:
    void startWatchingLastModifiedTimestampsFile(const QString &lastModifiedTimestampsFile);
private slots:
    void handleLastModifiedTimestampsChanged();
    void handleDeleteInFiveMinsTimerTimedOut();
};

#endif // LASTMODIFIEDTIMESTAMPSWATCHER_H
