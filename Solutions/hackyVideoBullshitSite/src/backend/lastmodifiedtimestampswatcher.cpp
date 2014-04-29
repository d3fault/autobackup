#include "lastmodifiedtimestampswatcher.h"

#include <QTimer>
#include <QMapIterator>
#include <QScopedPointer>
#include <QFileSystemWatcher>
#include <QStringList>

#include "lastmodifiedtimestampssorter.h"

LastModifiedTimestampsWatcher::LastModifiedTimestampsWatcher(QObject *parent)
    : QObject(parent)
    , m_LastModifiedTimestampsFileWatcher(0)
    , m_CurrentTimestampsAndPathsAtomicPointer(0)
    , m_FileWasMerelyModifiedNotOverwrittenSoWaitUntil1secondWithNoWritesTimer(new QTimer(this))
    , m_DeleteInFiveMinsTimer(new QTimer(this))
    , m_TimestampsAndPathsQueuedForDelete(0)
{
    m_FileWasMerelyModifiedNotOverwrittenSoWaitUntil1secondWithNoWritesTimer->setSingleShot(true);
    m_FileWasMerelyModifiedNotOverwrittenSoWaitUntil1secondWithNoWritesTimer->setInterval(1000);
    connect(m_FileWasMerelyModifiedNotOverwrittenSoWaitUntil1secondWithNoWritesTimer, SIGNAL(timeout()), this, SLOT(readLastModifiedTimestampsFile()));

    m_DeleteInFiveMinsTimer->setSingleShot(true);
    m_DeleteInFiveMinsTimer->setInterval(5*(1000*60));
    connect(m_DeleteInFiveMinsTimer, SIGNAL(timeout()), this, SLOT(handleDeleteInFiveMinsTimerTimedOut()));
}
QAtomicPointer<LastModifiedTimestampsAndPaths> *LastModifiedTimestampsWatcher::getTimestampsAndPathsAtomicPointer()
{
    return &m_CurrentTimestampsAndPathsAtomicPointer;
}
LastModifiedTimestampsWatcher::~LastModifiedTimestampsWatcher()
{
    if(m_TimestampsAndPathsQueuedForDelete)
    {
        while(!m_TimestampsAndPathsQueuedForDelete->isEmpty())
        {
            deleteOneTimestampAndPathQueuedForDelete();
        }
        delete m_TimestampsAndPathsQueuedForDelete;
    }
    LastModifiedTimestampsAndPaths *currentTimestampsAndPathsMaybe = m_CurrentTimestampsAndPathsAtomicPointer.fetchAndStoreOrdered(0);
    if(currentTimestampsAndPathsMaybe)
    {
        delete currentTimestampsAndPathsMaybe;
    }
    if(m_LastModifiedTimestampsFileWatcher)
    {
        delete m_LastModifiedTimestampsFileWatcher;
    }
}
void LastModifiedTimestampsWatcher::deleteOneTimestampAndPathQueuedForDelete()
{
    LastModifiedTimestampsAndPaths *currentToDelete = m_TimestampsAndPathsQueuedForDelete->dequeue();
    delete currentToDelete;
}
void LastModifiedTimestampsWatcher::startWatchingLastModifiedTimestampsFile(const QString &lastModifiedTimestampsFile)
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
    handleLastModifiedTimestampsChanged(); //populate at startup
    emit startedWatchingLastModifiedTimestampsFile();
}
void LastModifiedTimestampsWatcher::readLastModifiedTimestampsFile()
{
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
    LastModifiedTimestampsAndPaths *oldTimestampsAndPaths = m_CurrentTimestampsAndPathsAtomicPointer.fetchAndStoreOrdered(newTimestampsAndPaths); //TODOoptimization: might not need ordered, idfk
    if(!oldTimestampsAndPaths)
        return; //first time, nothing to queue for delete
    bool startTimerYea = m_TimestampsAndPathsQueuedForDelete->isEmpty();
    m_TimestampsAndPathsQueuedForDelete->enqueue(oldTimestampsAndPaths);
    if(startTimerYea) //don't double start (restart)
        m_DeleteInFiveMinsTimer->start();
}
void LastModifiedTimestampsWatcher::handleLastModifiedTimestampsChanged()
{
    //TO DOnereq: if i move an already finished version overwriting the old one, the old one is considered removed and will no longer be watched and needs to be re-added
    if(!m_LastModifiedTimestampsFileWatcher->files().isEmpty())
    {
        //not empty means that the "move" method wasn't used, so we want to wait 1 second after the last 'change' is seen before we start reading it
        //nvm we do want to restart it over and over: if(!m_FileWasMerelyModifiedNotOverwrittenSoWaitUntil1secondWithNoWritesTimer->isActive())
        m_FileWasMerelyModifiedNotOverwrittenSoWaitUntil1secondWithNoWritesTimer->start();
        return;
    }
    m_LastModifiedTimestampsFileWatcher->addPath(m_LastModifiedTimestampsFile);
    if(m_FileWasMerelyModifiedNotOverwrittenSoWaitUntil1secondWithNoWritesTimer->isActive()) //wtf? this would mean they did an append AND an overwrite/rename-onto
        return; //just return because yea the timeout will do it
    readLastModifiedTimestampsFile();
}
void LastModifiedTimestampsWatcher::handleDeleteInFiveMinsTimerTimedOut()
{
    if(m_TimestampsAndPathsQueuedForDelete->isEmpty())
        return;

    deleteOneTimestampAndPathQueuedForDelete();

    if(!m_TimestampsAndPathsQueuedForDelete->isEmpty())
        m_DeleteInFiveMinsTimer->start();
}
