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
    m_LastModifiedTimestampsFilesWatcher->addPath(lastModifiedTimestampsFile);

    LastModifiedTimestampsSorter lastModifiedTimestampsSorter(this);
    int totalPathsCount = 0;
    SortedMapOfListsOfPathsPointerType *oldUncombinedMap = m_LastModifiedTimestampsFilesAndPreCombinedSemiSortedTimestampsAndPathsMaps.value(lastModifiedTimestampsFile);
    delete oldUncombinedMap;
    m_LastModifiedTimestampsFilesAndPreCombinedSemiSortedTimestampsAndPathsMaps.insert(lastModifiedTimestampsFile, lastModifiedTimestampsSorter.sortLastModifiedTimestamps_ButDontSortPaths(lastModifiedTimestampsFile, &totalPathsCount, getPathPrefixForThisLastModifiedTimestampsFile(lastModifiedTimestampsFile))); //dont sort the paths because it would be a waste of time to sort them before merging with the others

    return true; //TODOoptional: above two ops can/should return false if fail
}
QString LastModifiedTimestampsWatcher::getPathPrefixForThisLastModifiedTimestampsFile(const QString &lastModifiedTimestampsFile)
{
    //TODOoptimization: *might* be worth it to cache this parsing... but eh fuck it pretty cheap anyways...
    QFileInfo lastModifiedTimestampsFilenameChopperOffer(lastModifiedTimestampsFile);
    QString absolutePath = appendSlashIfNeeded(lastModifiedTimestampsFilenameChopperOffer.absolutePath());
    absolutePath.remove(0, m_AbsolutePathToBaseViewDirSoTheFoldersInBetweenThereAndLastModifiedTimestampsFilesCanBeUsedAsPrefixInCombinedSortedList.length());
    return absolutePath;
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
//TO DOnereq: we need to keep the unresolved symlinks handy for re-adding. the key entry in QHash should probably be that symlink (since otherwise it'd be changing). HOWEVER, when qfsw tells us that a file is deleted, it is no longer the symlink. we resolved it! so i need to keep track of symlinks and resolvedz... ffff. wait a tick, i don't need to resolve it in the first place and then it's no longer a problem... why was i anyways? i guess it was just to help me with understanding...
void LastModifiedTimestampsWatcher::startWatchingLastModifiedTimestampsFiles(const QString &absolutePathToBaseViewDirSoTheFoldersInBetweenThereAndLastModifiedTimestampsFilesCanBeUsedAsPrefixInCombinedSortedList, const QStringList &lastModifiedTimestampsFiles)
{
    m_AbsolutePathToBaseViewDirSoTheFoldersInBetweenThereAndLastModifiedTimestampsFilesCanBeUsedAsPrefixInCombinedSortedList = appendSlashIfNeeded(absolutePathToBaseViewDirSoTheFoldersInBetweenThereAndLastModifiedTimestampsFilesCanBeUsedAsPrefixInCombinedSortedList);
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
        if(!lastModifiedTimestampsFile.startsWith(m_AbsolutePathToBaseViewDirSoTheFoldersInBetweenThereAndLastModifiedTimestampsFilesCanBeUsedAsPrefixInCombinedSortedList))
        {
            emit e("error: '" + lastModifiedTimestampsFile + "' is not in a sub-folder of: '" + m_AbsolutePathToBaseViewDirSoTheFoldersInBetweenThereAndLastModifiedTimestampsFilesCanBeUsedAsPrefixInCombinedSortedList + "'");
            return; //TODOoptional: this and the below return should return false, and that false should be emitted back to caller and exit cleanly etc (whereas now hvbs just.. err... stops mid-startup without the startedWatchingLastModifiedTimestampsFile signal..)
        }
        m_LastModifiedTimestampsFilesAndPreCombinedSemiSortedTimestampsAndPathsMaps.insert(lastModifiedTimestampsFile, new SortedMapOfListsOfPathsPointerType());
        if(!addAndReadLastModifiedTimestampsFile(lastModifiedTimestampsFile)) //populate at startup
            return;
    }
    combineAndPublishLastModifiedTimestampsFiles();
    emit startedWatchingLastModifiedTimestampsFile();
}
void LastModifiedTimestampsWatcher::handleLastModifiedTimestampsChanged(const QString &lastModifiedTimestampsFileThatChanged)
{
    if(!addAndReadLastModifiedTimestampsFile(lastModifiedTimestampsFileThatChanged))
        return;
    combineAndPublishLastModifiedTimestampsFiles(); //TODOoptimization: maybe another one needs reading (maybe they all changed at, or around, the same time)... so it would be an optimization to not combine yet if that were the case and, more importantly, it is detectable. perhaps clever queued combine invocation (0 length timer) hackery with some kind of boolean flag that somehow cancels it (stop the timer?). not sure if canceling a timer that has "already fired" will stop the slot... could test that though..
}
void LastModifiedTimestampsWatcher::handleDeleteInFiveMinsTimerTimedOut()
{
    if(m_TimestampsAndPathsQueuedForDelete->isEmpty())
        return;

    deleteOneTimestampAndPathQueuedForDelete();

    if(!m_TimestampsAndPathsQueuedForDelete->isEmpty())
        m_DeleteInFiveMinsTimer->start();
}
