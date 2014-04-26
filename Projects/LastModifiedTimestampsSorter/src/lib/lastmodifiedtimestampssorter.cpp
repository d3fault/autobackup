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
LastModifiedTimestampsSorter::~LastModifiedTimestampsSorter()
{
    if(m_MapOfPathsListsSortedByModificationDate)
    {
        clearTheMapIfNeeded();
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
void LastModifiedTimestampsSorter::clearTheMapIfNeeded()
{
    QMapIterator<long long, QList<std::string>* > mapOfPathListsIterator(*m_MapOfPathsListsSortedByModificationDate);
    while(mapOfPathListsIterator.hasNext())
    {
        mapOfPathListsIterator.next();
        QList<std::string> *listOfPaths = mapOfPathListsIterator.value();
        delete listOfPaths;
    }
    m_MapOfPathsListsSortedByModificationDate->clear();
}
void LastModifiedTimestampsSorter::sortAllLastModifiedTimestampsOnIoDevice(QIODevice *lastModifiedTimestampsIoDevice)
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

        long long currentTimestamp = timestamp.lastModifiedTimestamp().toMSecsSinceEpoch()/1000;
        if(!m_MapOfPathsListsSortedByModificationDate->contains(currentTimestamp))
        {
            QList<std::string> *newListOfPaths = new QList<std::string>();
            newListOfPaths->append(timestamp.filePath().toStdString());
            m_MapOfPathsListsSortedByModificationDate->insert(currentTimestamp, newListOfPaths);
        }
        else
        {
            QList<std::string> *listOfPaths = m_MapOfPathsListsSortedByModificationDate->value(currentTimestamp);
            listOfPaths->append(timestamp.filePath().toStdString()); //TODOoptional: could make sure the path isn't already there (otherwise we'd be silently overwriting), BUT it's overkill since the generator of said list will not do that... and besides it'd be an EXACT dupe because the timestamp would be the same... so dropping it makes sense anyways (still, a warning wouldn't hurt (unless parsing stdout/stderr and well yea you get the picture :-P))
        }
    }

    //the keys were sorted as we added them to the map, but the values (lists of paths) still need to be sorted
    QMapIterator<long long, QList<std::string>* > mapOfListsOfPathsIterator(*m_MapOfPathsListsSortedByModificationDate);
    while(mapOfListsOfPathsIterator.hasNext())
    {
        mapOfListsOfPathsIterator.next();
        QList<std::string> *listOfPaths = mapOfListsOfPathsIterator.value();
        std::sort(listOfPaths->begin(), listOfPaths->end());
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
    clearTheMapIfNeeded();
    sortAllLastModifiedTimestampsOnIoDevice(lastModifiedTimestampsIoDevice);
    emit lastModifiedTimestampsSorted(takeMapOfPathsListsSortedByModificationDate());
}
void LastModifiedTimestampsSorter::sortAndEmitLastModifiedTimestamps(const QString &lastModifiedTimestampsFilename)
{
    QFile lastModifiedTimestampsFile(lastModifiedTimestampsFilename, this);
    if(!lastModifiedTimestampsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("failed to open for reading: " + lastModifiedTimestampsFilename);
        emit lastModifiedTimestampsSorted(0);
        return;
    }
    sortAndEmitLastModifiedTimestamps(&lastModifiedTimestampsFile);
}
void LastModifiedTimestampsSorter::sortAndOutputLastModifiedTimestamps(QIODevice *lastModifiedTimestampsIoDevice)
{
    newTheMapIfNeeded();
    clearTheMapIfNeeded();
    sortAllLastModifiedTimestampsOnIoDevice(lastModifiedTimestampsIoDevice);
    outputTheMap(); //"cli"/user of this should do the formatting, but w/e
    emit finishedOutputtingSortedLines();
}
void LastModifiedTimestampsSorter::sortAndOutputLastModifiedTimestamps(const QString &lastModifiedTimestampsFilename)
{
    QFile lastModifiedTimestampsFile(lastModifiedTimestampsFilename, this);
    if(!lastModifiedTimestampsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("failed to open for reading: " + lastModifiedTimestampsFilename);
        emit finishedOutputtingSortedLines();
        return;
    }
    sortAndOutputLastModifiedTimestamps(&lastModifiedTimestampsFile);
}
