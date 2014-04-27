#include "lastmodifiedtimestampswatcher.h"

#include <QTimer>
#include <QMapIterator>
#include <QScopedPointer>
#include <QFileSystemWatcher>

#include "lastmodifiedtimestampssorter.h"

LastModifiedTimestampsWatcher::LastModifiedTimestampsWatcher(QObject *parent)
    : QObject(parent)
    , m_LastModifiedTimestampsFileWatcher(0)
    , m_DeleteInFiveMinsTimer(new QTimer(this))
    , m_TimestampsAndPathsQueuedForDelete(0)
{
    m_DeleteInFiveMinsTimer->setSingleShot(true);
    m_DeleteInFiveMinsTimer->setInterval(5*(1000*60));
    connect(m_DeleteInFiveMinsTimer, SIGNAL(timeout()), this, SLOT(handleDeleteInFiveMinsTimerTimedOut()));
}
LastModifiedTimestampsWatcher::~LastModifiedTimestampsWatcher()
{
    finishStopping();
}
void LastModifiedTimestampsWatcher::deleteOneTimestampAndPathQueuedForDelete()
{
    LastModifiedTimestampsAndPaths *currentToDelete = m_TimestampsAndPathsQueuedForDelete->dequeue();
    delete currentToDelete;
}
void LastModifiedTimestampsWatcher::startWatchingLastModifiedTimestampsFile(const QString &lastModifiedTimestampsFile, QAtomicPointer<LastModifiedTimestampsAndPaths> *timestampsAndPathsSharedAtomicPointer)
{
    m_LastModifiedTimestampsFile = lastModifiedTimestampsFile;
    if(!m_LastModifiedTimestampsFileWatcher)
    {
        m_LastModifiedTimestampsFileWatcher = new QFileSystemWatcher(this);
        connect(m_LastModifiedTimestampsFileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(handleLastModifiedTimestampsChanged()));
    }
    if(!m_TimestampsAndPathsQueuedForDelete)
    {
        m_TimestampsAndPathsQueuedForDelete = new QQueue<LastModifiedTimestampsAndPaths*>();
    }
    m_CurrentTimestampsAndPathsAtomicPointer = timestampsAndPathsSharedAtomicPointer;
}
//safe to call twice
void LastModifiedTimestampsWatcher::finishStopping()
{
    if(m_TimestampsAndPathsQueuedForDelete)
    {
        while(!m_TimestampsAndPathsQueuedForDelete->isEmpty())
        {
            deleteOneTimestampAndPathQueuedForDelete();
        }
        delete m_TimestampsAndPathsQueuedForDelete;
        m_TimestampsAndPathsQueuedForDelete = 0;
    }
    LastModifiedTimestampsAndPaths *currentTimestampsAndPathsMaybe = m_CurrentTimestampsAndPathsAtomicPointer->fetchAndStoreOrdered(0);
    if(currentTimestampsAndPathsMaybe)
    {
        delete currentTimestampsAndPathsMaybe;
    }
    if(m_LastModifiedTimestampsFileWatcher)
    {
        delete m_LastModifiedTimestampsFileWatcher;
        m_LastModifiedTimestampsFileWatcher = 0;
    }
}
void LastModifiedTimestampsWatcher::handleLastModifiedTimestampsChanged()
{
    //TO DOnereq: if i move an already finished version overwriting the old one, the old one is considered removed and will no longer be watched and needs to be re-added
    m_LastModifiedTimestampsFileWatcher->addPath(m_LastModifiedTimestampsFile);
    //TODOreq: we either need to ensure that the .lastModifiedTimestamps file is only ever "moved onto" (rename already finished file to it), or we can start a singleshot timer to come back in 5 seconds (and perhaps keep restarting it to 5 seconds xD? that might be expensive though (but safer)) so that the writes all finish before we begin sorting. "moving onto" method sounds easier/safer tbh
    //TODOoptional: could check to see if our path is still being watched. if it is, we check back in 5 seconds or so. if it ISN'T, we know that a move/overwrite method was used and re-add it. best of both worlds...

    LastModifiedTimestampsSorter lastModifiedTimestampsSorter(this);
    int totalPathsCount = 0;
    QScopedPointer<SortedMapOfListsOfPathsPointerType> sortedMap(lastModifiedTimestampsSorter.sortLastModifiedTimestamps(m_LastModifiedTimestampsFile, &totalPathsCount)); //the values stay alive when the pointer goes out of scope, which is good :)

    //now we convert it to two formats we want

    QList<TimestampAndPath*> *sortedTimestampAndPathFlatList = new QList<TimestampAndPath*>();
    sortedTimestampAndPathFlatList->reserve(totalPathsCount);
    int currentIndexIntoSortedPathsFlatList = -1;
    PathsIndexIntoFlatListHashType *pathsIndexIntoFlatListHash = new PathsIndexIntoFlatListHashType();
    pathsIndexIntoFlatListHash->reserve(totalPathsCount);


    //TO DOnereq: skip over directory entries (which may be the only entry for that timestamp), honestly i don't think they're even that necessary (although creating empty directories to indicate a project i want to start is kinda handy), maybe i should just stop keeping track of them instead...

    QMapIterator<long long, QList<std::string>* > sortedMapIterator(*sortedMap.data());
    while(sortedMapIterator.hasNext())
    {
        sortedMapIterator.next();
        QList<std::string> *allPathsWithThisTimestamp = sortedMapIterator.value();
        QListIterator<std::string> allPathsWithThisTimestampIterator(*allPathsWithThisTimestamp);
        while(allPathsWithThisTimestampIterator.hasNext())
        {
            std::string currentPath = "/" /* convert to internal path, which starts with a slash */ + allPathsWithThisTimestampIterator.next();
            const QString &currentPathQString = QString::fromStdString(currentPath);
            if(currentPathQString.endsWith("/") /*we don't want directories*/ || currentPathQString == "/"/*it was an empty path (error)*/)
                continue;

            TimestampAndPath *timestampAndPath = new TimestampAndPath(sortedMapIterator.key(), currentPath);
            sortedTimestampAndPathFlatList->append(timestampAndPath);
            (*pathsIndexIntoFlatListHash)[currentPath] = ++currentIndexIntoSortedPathsFlatList; //funny, if i use QString then the string is doubled in size (16-bit vs. 8-bit), and yet QString also saves me half the space because of implicit sharing xD...
        }
    }

    LastModifiedTimestampsAndPaths *newTimestampsAndPaths = new LastModifiedTimestampsAndPaths(sortedTimestampAndPathFlatList, pathsIndexIntoFlatListHash);
    if(sortedTimestampAndPathFlatList->isEmpty())
    {
        delete newTimestampsAndPaths; //chances are the list that they already have is better than an empty list...
        return;
    }
    LastModifiedTimestampsAndPaths *oldTimestampsAndPaths = m_CurrentTimestampsAndPathsAtomicPointer->fetchAndStoreOrdered(newTimestampsAndPaths); //TODOoptimization: might not need ordered, idfk
    if(!oldTimestampsAndPaths)
        return;
    bool startTimerYea = m_TimestampsAndPathsQueuedForDelete->isEmpty();
    m_TimestampsAndPathsQueuedForDelete->enqueue(oldTimestampsAndPaths);
    if(startTimerYea) //don't double start (restart)
        m_DeleteInFiveMinsTimer->start();
}
void LastModifiedTimestampsWatcher::handleDeleteInFiveMinsTimerTimedOut()
{
    if(m_TimestampsAndPathsQueuedForDelete->isEmpty())
        return;

    deleteOneTimestampAndPathQueuedForDelete();

    if(!m_TimestampsAndPathsQueuedForDelete->isEmpty())
        m_DeleteInFiveMinsTimer->start();
}
