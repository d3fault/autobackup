#include "lastmodifiedtimestampswatcher.h"

#include <QTimer>
#include <QMapIterator>
#include <QScopedPointer>
#include <QFileSystemWatcher>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QHashIterator>

//git push -> post-update -> git clone/archive -> symlinkSwap -> deleteOldLastModifiedJustToTriggerQfsWatcher (also delete rest of shit) -> [re-]resolveAndWatchSymlink/.lastModified

LastModifiedTimestampsWatcher::LastModifiedTimestampsWatcher(QObject *parent)
    : QObject(parent)
    , m_LastModifiedTimestampsFilesWatcher(0)
    , m_CurrentTimestampsAndPathsAtomicPointer(0) //initialize to value zero (which coincidentally is a null/0 ptr address), not null/0 ptr address [of the member itself]
    , m_DeleteInFiveMinsTimer(new QTimer(this))
    , m_TimestampsAndPathsQueuedForDelete(0)
{
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
    if(m_LastModifiedTimestampsFilesWatcher)
    {
        delete m_LastModifiedTimestampsFilesWatcher;
    }
}
bool LastModifiedTimestampsWatcher::addAndReadLastModifiedTimestampsFile(const QString &lastModifiedTimestampsFile)
{
    if(!QFile::exists(lastModifiedTimestampsFile))
    {
        emit e("TOTAL SYSTEM FAILURE: you did not use 'move [symlink] atomics' for last modified timestamp file: " + lastModifiedTimestampsFile); //TODOoptional: error out etcz
        return false;
    }
    resolveLastModifiedTimestampsFilePathAndWatchIt(lastModifiedTimestampsFile);
    readLastModifiedTimestampsFile(lastModifiedTimestampsFile);
    return true; //TODOoptional: above two methods can return bool
}
void LastModifiedTimestampsWatcher::resolveLastModifiedTimestampsFilePathAndWatchIt(const QString &lastModifiedTimestampsFile)
{
    QFileInfo symlinkResolver(lastModifiedTimestampsFile);
    m_LastModifiedTimestampsFilesWatcher->addPath(symlinkResolver.canonicalFilePath());
}
void LastModifiedTimestampsWatcher::combineAndPublishLastModifiedTimestampsFiles()
{
    //combine with other maps before converting to flat list and paths index into flat list
    QScopedPointer<SortedMapOfListsOfPathsPointerType> combinedMap(new SortedMapOfListsOfPathsPointerType());
    QHashIterator<QString, SortedMapOfListsOfPathsPointerType*> uncombinedMapsIterator(m_LastModifiedTimestampsFilesAndPreCombinedSemiSortedTimestampsAndPathsMaps);
    while(uncombinedMapsIterator.hasNext())
    {
        uncombinedMapsIterator.next();
        QMapIterator<long long, QList<std::string>* > mapCurrentlyBeingInsertedIntoCombinedMapIterator(*uncombinedMapsIterator.value());
        while(mapCurrentlyBeingInsertedIntoCombinedMapIterator.hasNext())
        {
            mapCurrentlyBeingInsertedIntoCombinedMapIterator.next();
            QList<std::string>* maybeExistingPathsForTimestamp = combinedMap->value(mapCurrentlyBeingInsertedIntoCombinedMapIterator.key(), 0);
            if(maybeExistingPathsForTimestamp)
            {
                //key already exists in combined map
                maybeExistingPathsForTimestamp->append(*mapCurrentlyBeingInsertedIntoCombinedMapIterator.value()); //TODOreq: wtf is the memory shit of this? maybe just fuck it and let valgrind tell me :). starting to think i should have used qstring since implicitly (we now have two copies :-/) shared and simplified...
            }
            else
            {
                //key does not yet exist in combined map
                combinedMap->insert(mapCurrentlyBeingInsertedIntoCombinedMapIterator.key(), mapCurrentlyBeingInsertedIntoCombinedMapIterator.value()); //TODOreq: ditto about memory confusion
            }
        }
    }

    //now we convert it to two formats we want
    QList<TimestampAndPath*> *sortedTimestampAndPathFlatList = new QList<TimestampAndPath*>();
    //sortedTimestampAndPathFlatList->reserve(totalPathsCount); //TODOoptimization: maybe re-use these values again
    int currentIndexIntoSortedPathsFlatList = -1;
    PathsIndexIntoFlatListHashType *pathsIndexIntoFlatListHash = new PathsIndexIntoFlatListHashType();
    //pathsIndexIntoFlatListHash->reserve(totalPathsCount);


    //TO DOnereq: skip over directory entries (which may be the only entry for that timestamp), honestly i don't think they're even that necessary (although creating empty directories to indicate a project i want to start is kinda handy), maybe i should just stop keeping track of them instead...

    QMapIterator<long long, QList<std::string>* > combineddMapIterator(*combinedMap.data());
    while(combineddMapIterator.hasNext())
    {
        combineddMapIterator.next();
        QList<std::string> *allPathsWithThisTimestamp = combineddMapIterator.value();

        //a step we skipped earlier was that each of the paths (values of map) weren't being sorted. now that they're all combined, we sort them
        std::sort(allPathsWithThisTimestamp->begin(), allPathsWithThisTimestamp->end());

        QListIterator<std::string> allPathsWithThisTimestampIterator(*allPathsWithThisTimestamp);
        while(allPathsWithThisTimestampIterator.hasNext())
        {
            std::string currentPath = "/" /* convert to internal path, which starts with a slash */ + allPathsWithThisTimestampIterator.next();
            const QString &currentPathQString = QString::fromStdString(currentPath);
            if(currentPathQString.endsWith("/") /*we don't want directories*/ || currentPathQString == "/"/*it was an empty path (error)*/)
                continue;

            TimestampAndPath *timestampAndPath = new TimestampAndPath(combineddMapIterator.key(), currentPath);
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
    if(m_TimestampsAndPathsQueuedForDelete->isEmpty()) //don't double start (restart)
        m_DeleteInFiveMinsTimer->start();
    m_TimestampsAndPathsQueuedForDelete->enqueue(oldTimestampsAndPaths);
}
void LastModifiedTimestampsWatcher::deleteOneTimestampAndPathQueuedForDelete()
{
    LastModifiedTimestampsAndPaths *currentToDelete = m_TimestampsAndPathsQueuedForDelete->dequeue();
    delete currentToDelete;
}
void LastModifiedTimestampsWatcher::startWatchingLastModifiedTimestampsFile(const QStringList &lastModifiedTimestampsFiles)
{
    if(m_LastModifiedTimestampsFilesWatcher)
        delete m_LastModifiedTimestampsFilesWatcher;
    m_LastModifiedTimestampsFilesWatcher = new QFileSystemWatcher(this);
    connect(m_LastModifiedTimestampsFilesWatcher, SIGNAL(fileChanged(QString)), this, SLOT(handleLastModifiedTimestampsChanged(QString)));
    if(!m_TimestampsAndPathsQueuedForDelete)
    {
        m_TimestampsAndPathsQueuedForDelete = new QQueue<LastModifiedTimestampsAndPaths*>();
    }
    foreach(const QString &lastModifiedTimestampsFile, lastModifiedTimestampsFiles)
    {
        m_LastModifiedTimestampsFilesAndPreCombinedSemiSortedTimestampsAndPathsMaps.insert(lastModifiedTimestampsFile, new SortedMapOfListsOfPathsPointerType());
        if(!addAndReadLastModifiedTimestampsFile(lastModifiedTimestampsFile)) //populate at startup
            return;
    }
    combineAndPublishLastModifiedTimestampsFiles();
    emit startedWatchingLastModifiedTimestampsFile();
}
void LastModifiedTimestampsWatcher::readLastModifiedTimestampsFile(const QString &lastModifiedTimestampsFile)
{
    LastModifiedTimestampsSorter lastModifiedTimestampsSorter(this);
    int totalPathsCount = 0;

    SortedMapOfListsOfPathsPointerType *oldUncombinedMap = m_LastModifiedTimestampsFilesAndPreCombinedSemiSortedTimestampsAndPathsMaps.value(lastModifiedTimestampsFile);
    delete oldUncombinedMap;
    m_LastModifiedTimestampsFilesAndPreCombinedSemiSortedTimestampsAndPathsMaps.insert(lastModifiedTimestampsFile,lastModifiedTimestampsSorter.sortLastModifiedTimestamps_ButDontSortPaths(lastModifiedTimestampsFile, &totalPathsCount)); //dont sort the paths because it would be a waste of time to sort them before merging with the others
}
void LastModifiedTimestampsWatcher::handleLastModifiedTimestampsChanged(const QString &lastModifiedTimestampsFileThatChanged)
{
    if(!addAndReadLastModifiedTimestampsFile(lastModifiedTimestampsFileThatChanged))
        return;
    combineAndPublishLastModifiedTimestampsFiles();
}
void LastModifiedTimestampsWatcher::handleDeleteInFiveMinsTimerTimedOut()
{
    if(m_TimestampsAndPathsQueuedForDelete->isEmpty())
        return;

    deleteOneTimestampAndPathQueuedForDelete();

    if(!m_TimestampsAndPathsQueuedForDelete->isEmpty())
        m_DeleteInFiveMinsTimer->start();
}
