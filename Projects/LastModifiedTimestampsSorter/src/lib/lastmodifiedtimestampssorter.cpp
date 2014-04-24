#include "lastmodifiedtimestampssorter.h"

#include <QTextStream>
#include <QScopedPointer>
#include <QMultiMap>
#include <QMapIterator>

#include "simplifiedlastmodifiedtimestamp.h"

#define LAST_MODIFIED_TIMESTAMP_SORTER_SORT_IDENTICAL_TIMESTAMPS_BY_PATH

LastModifiedTimestampsSorter::LastModifiedTimestampsSorter(QObject *parent) :
    QObject(parent)
{ }
void LastModifiedTimestampsSorter::formatAndOutputSortedLine(long long timestamp, const QString &path)
{
    emit sortedLineOutput(QString::number(timestamp) + " - " + path); //i could make this a colonSeparated line but it would just add confusion etc. maybe in hindsight i should have sorted by that to begin with (but really sorting alphabetically makes more sense when iterating a filesystem (can't [easily(cheaply?)] do sort by timestamp for multiple levels of directories...). Now considering even more doing this as colon separated line since I might integrate it into HackyVideoBullshitSite (or perhaps add it as a step to QuickDirtyAutoBackupHalper)
}
void LastModifiedTimestampsSorter::sortAndOutputLastModifiedTimestamps(QIODevice *lastModifiedTimestampsIoDevice)
{
    QTextStream lastModifiedTimestampsTextStream(lastModifiedTimestampsIoDevice);
    QScopedPointer<QMultiMap<long long, QString> > multiMap(new QMultiMap<long long, QString>()); //heap because it will be huge...
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
        multiMap->insert(timestamp.lastModifiedTimestamp().toMSecsSinceEpoch()/1000, timestamp.filePath());
    }

#ifdef LAST_MODIFIED_TIMESTAMP_SORTER_SORT_IDENTICAL_TIMESTAMPS_BY_PATH
    //we used a multimap to do the sorting, now we want to sort the identical timestamps by path and then output each item
    QList<long long> uniqueTimestampKeys = multiMap->uniqueKeys();
    QListIterator<long long> uniqueTimestampKeysIterator(uniqueTimestampKeys);
    while(uniqueTimestampKeysIterator.hasNext())
    {
        long long currentTimestamp = uniqueTimestampKeysIterator.next();
        QList<QString> pathsWithThisTimestamp = multiMap->values(currentTimestamp);
        std::sort(pathsWithThisTimestamp.begin(), pathsWithThisTimestamp.end());
        QListIterator<QString> pathsWithThisTimestampNowSortedByPathIterator(pathsWithThisTimestamp);
        while(pathsWithThisTimestampNowSortedByPathIterator.hasNext())
        {
            formatAndOutputSortedLine(currentTimestamp, pathsWithThisTimestampNowSortedByPathIterator.next());
        }
    }
#else
    //we used a MultiMap to do the sorting, now we iterate it and output each item
    QMapIterator<long long, QString> it(*multiMap.data()); //deref the deref? wot
    while(it.hasNext())
    {
        it.next();

        //oops idk why i did this (and even implement toColon (which i thought was bugged but was just unreferenced lol)
        //SimplifiedLastModifiedTimestamp timestamp(it.value(), QDateTime::fromMSecsSinceEpoch(it.key()*1000));
        //emit d(SimplifiedLastModifiedTimestamp)
        formatAndOutputSortedLine(it.key(), it.value());
    }
#endif

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
    lastModifiedTimestampsFile.close();
}
