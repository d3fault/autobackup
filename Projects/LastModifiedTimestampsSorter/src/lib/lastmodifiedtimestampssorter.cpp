#include "lastmodifiedtimestampssorter.h"

#include <QTextStream>
#include <QMapIterator>
#include <QListIterator>
#include <QFile>
#include <QIODevice>

#include "simplifiedlastmodifiedtimestamp.h"

LastModifiedTimestampsSorter::LastModifiedTimestampsSorter(QObject *parent)
    : QObject(parent)
    , m_MapOfPathsListsSortedByModificationDate(0)
{ }
SortedMapOfListsOfPathsPointerType *LastModifiedTimestampsSorter::takeMapOfPathsListsSortedByModificationDate()
{
    if(m_MapOfPathsListsSortedByModificationDate)
    {
        SortedMapOfListsOfPathsPointerType *ret = m_MapOfPathsListsSortedByModificationDate;
        m_MapOfPathsListsSortedByModificationDate = 0;
        return ret;
    }
    return 0;
}
SortedMapOfListsOfPathsPointerType *LastModifiedTimestampsSorter::sortLastModifiedTimestamps_ButDontSortPaths(QIODevice *lastModifiedTimestampsIoDevice, int *itemsCount)
{
    newTheMapIfNeeded();
    clearTheMap();
    sortAllLastModifiedTimestampsKeysAkaTimestampsOnlyOnIoDevice(lastModifiedTimestampsIoDevice);
    if(itemsCount)
        *itemsCount = m_TotalItemsCount;
    return takeMapOfPathsListsSortedByModificationDate();
}
SortedMapOfListsOfPathsPointerType *LastModifiedTimestampsSorter::sortLastModifiedTimestamps_ButDontSortPaths(const QString &lastModifiedTimestampsFilename, int *itemsCount)
{
    QFile lastModifiedTimestampsFile(lastModifiedTimestampsFilename, this);
    if(!lastModifiedTimestampsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("failed to open for reading: " + lastModifiedTimestampsFilename);
        return 0;
    }
    return sortLastModifiedTimestamps_ButDontSortPaths(&lastModifiedTimestampsFile, itemsCount);
}
SortedMapOfListsOfPathsPointerType *LastModifiedTimestampsSorter::sortLastModifiedTimestamps(QIODevice *lastModifiedTimestampsIoDevice, int *itemsCount)
{
    newTheMapIfNeeded();
    clearTheMap();
    sortAllLastModifiedTimestampsKeysAkaTimestampsOnlyOnIoDevice(lastModifiedTimestampsIoDevice);
    sortAllLastModifiedTimestampsValuesAkaPathsOnInternalMap();
    if(itemsCount)
        *itemsCount = m_TotalItemsCount;
    return takeMapOfPathsListsSortedByModificationDate();
}
SortedMapOfListsOfPathsPointerType *LastModifiedTimestampsSorter::sortLastModifiedTimestamps(const QString &lastModifiedTimestampsFilename, int *itemsCount)
{
    QFile lastModifiedTimestampsFile(lastModifiedTimestampsFilename, this);
    if(!lastModifiedTimestampsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("failed to open for reading: " + lastModifiedTimestampsFilename);
        return 0;
    }
    return sortLastModifiedTimestamps(&lastModifiedTimestampsFile, itemsCount);
}
LastModifiedTimestampsSorter::~LastModifiedTimestampsSorter()
{
    if(m_MapOfPathsListsSortedByModificationDate)
    {
        clearTheMap();
        delete m_MapOfPathsListsSortedByModificationDate;
    }
}
void LastModifiedTimestampsSorter::newTheMapIfNeeded()
{
    if(!m_MapOfPathsListsSortedByModificationDate)
    {
        m_MapOfPathsListsSortedByModificationDate = new SortedMapOfListsOfPathsPointerType();
    }
}
void LastModifiedTimestampsSorter::clearTheMap()
{
    qDeleteAll(m_MapOfPathsListsSortedByModificationDate->begin(), m_MapOfPathsListsSortedByModificationDate->end());
    m_MapOfPathsListsSortedByModificationDate->clear();
    m_TotalItemsCount = 0;
}
void LastModifiedTimestampsSorter::sortAllLastModifiedTimestampsKeysAkaTimestampsOnlyOnIoDevice(QIODevice *lastModifiedTimestampsIoDevice)
{
    QTextStream lastModifiedTimestampsTextStream(lastModifiedTimestampsIoDevice);
    //QScopedPointer<QMultiMap<long long, QString> > multiMap(new QMultiMap<long long, QString>()); //heap because it will be huge...

    while(!lastModifiedTimestampsTextStream.atEnd())
    {
        QString currentLine = lastModifiedTimestampsTextStream.readLine();
        if(currentLine.isEmpty())
            continue;
        SimplifiedLastModifiedTimestamp timestamp(currentLine);
        if(!timestamp.isValid())
        {
            emit d("Invalid SimplifiedLastModifiedTimestamp: " + currentLine);
            continue; //TODOoptional: "-quitOnError" arg, just "return;" here (but also don't return 0 in main) instead of "continue;"
        }
        ++m_TotalItemsCount;
        long long currentTimestamp = timestamp.lastModifiedTimestamp().toMSecsSinceEpoch()/1000;
        QList<std::string> *listOfPaths = m_MapOfPathsListsSortedByModificationDate->value(currentTimestamp, 0);
        if(!listOfPaths)
        {
            //not yet in map
            QList<std::string> *newListOfPaths = new QList<std::string>();
            newListOfPaths->append(timestamp.filePath().toStdString());
            m_MapOfPathsListsSortedByModificationDate->insert(currentTimestamp, newListOfPaths);
        }
        else
        {
            //already in map
            QList<std::string> *listOfPaths = m_MapOfPathsListsSortedByModificationDate->value(currentTimestamp);
            listOfPaths->append(timestamp.filePath().toStdString()); //TODOoptional: could make sure the path isn't already there (otherwise we'd be silently overwriting), BUT it's overkill since the generator of said list will not do that... and besides it'd be an EXACT dupe because the timestamp would be the same... so dropping it makes sense anyways (still, a warning wouldn't hurt (unless parsing stdout/stderr and well yea you get the picture :-P))
        }
    }
}
void LastModifiedTimestampsSorter::sortAllLastModifiedTimestampsValuesAkaPathsOnInternalMap()
{
    //the keys were sorted as we added them to the map, but the values (lists of paths) still need to be sorted
    QMapIterator<long long, QList<std::string>* > mapOfListsOfPathsIterator(*m_MapOfPathsListsSortedByModificationDate);
    while(mapOfListsOfPathsIterator.hasNext())
    {
        mapOfListsOfPathsIterator.next();
        QList<std::string> *listOfPaths = mapOfListsOfPathsIterator.value();
        std::sort(listOfPaths->begin(), listOfPaths->end()); //for some reason this reminds me of a shimmy shuffle sort. or like that guy on the dude polishing his bowling ball
    }
}
void LastModifiedTimestampsSorter::outputTheMap()
{
    QMapIterator<long long, QList<std::string>* > mapOfListsOfPathsIterator(*m_MapOfPathsListsSortedByModificationDate);
    while(mapOfListsOfPathsIterator.hasNext())
    {
        mapOfListsOfPathsIterator.next();
        const QString &currentTimestampString = QString::number(mapOfListsOfPathsIterator.key());
        QListIterator<std::string> listOfPathsIterator(*mapOfListsOfPathsIterator.value());
        while(listOfPathsIterator.hasNext())
        {

            emit sortedLineOutput(currentTimestampString + " - " + QString::fromStdString(listOfPathsIterator.next())); //i could make this a colonSeparated line but it would just add confusion etc. maybe in hindsight i should have sorted by that to begin with (but really sorting alphabetically makes more sense when iterating a filesystem (can't [easily(cheaply?)] do sort by timestamp for multiple levels of directories...). Now considering even more doing this as colon separated line since I might integrate it into HackyVideoBullshitSite (or perhaps add it as a step to QuickDirtyAutoBackupHalper)
        }
    }
}
void LastModifiedTimestampsSorter::sortAndEmitLastModifiedTimestamps(QIODevice *lastModifiedTimestampsIoDevice)
{
    newTheMapIfNeeded();
    clearTheMap();
    sortAllLastModifiedTimestampsKeysAkaTimestampsOnlyOnIoDevice(lastModifiedTimestampsIoDevice);
    sortAllLastModifiedTimestampsValuesAkaPathsOnInternalMap();
    emit lastModifiedTimestampsSorted(takeMapOfPathsListsSortedByModificationDate(), m_TotalItemsCount);
}
void LastModifiedTimestampsSorter::sortAndEmitLastModifiedTimestamps(const QString &lastModifiedTimestampsFilename)
{
    QFile lastModifiedTimestampsFile(lastModifiedTimestampsFilename, this);
    if(!lastModifiedTimestampsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("failed to open for reading: " + lastModifiedTimestampsFilename);
        emit lastModifiedTimestampsSorted(0, 0);
        return;
    }
    sortAndEmitLastModifiedTimestamps(&lastModifiedTimestampsFile);
}
void LastModifiedTimestampsSorter::sortAndOutputLastModifiedTimestamps(QIODevice *lastModifiedTimestampsIoDevice)
{
    newTheMapIfNeeded();
    clearTheMap();
    sortAllLastModifiedTimestampsKeysAkaTimestampsOnlyOnIoDevice(lastModifiedTimestampsIoDevice);
    sortAllLastModifiedTimestampsValuesAkaPathsOnInternalMap();
    outputTheMap(); //"cli"/user of this should do the formatting, but w/e
    emit finishedOutputtingSortedLines(m_TotalItemsCount);
}
void LastModifiedTimestampsSorter::sortAndOutputLastModifiedTimestamps(const QString &lastModifiedTimestampsFilename)
{
    QFile lastModifiedTimestampsFile(lastModifiedTimestampsFilename, this);
    if(!lastModifiedTimestampsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("failed to open for reading: " + lastModifiedTimestampsFilename);
        emit finishedOutputtingSortedLines(0);
        return;
    }
    sortAndOutputLastModifiedTimestamps(&lastModifiedTimestampsFile);
}
