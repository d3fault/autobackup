#include "lastmodifiedtimestampssorter.h"

#include <QTextStream>
#include <QScopedPointer>
#include <QMultiMap>
#include <QMapIterator>

#include "simplifiedlastmodifiedtimestamp.h"

LastModifiedTimestampsSorter::LastModifiedTimestampsSorter(QObject *parent) :
    QObject(parent)
{ }
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
        multiMap->insert(timestamp.lastModifiedTimestamp().toMSecsSinceEpoch()/1000, timestamp.filePath());
    }

    //we used a MultiMap to do the sorting, now we iterate it and output each item

    QMapIterator<long long, QString> it(*multiMap.data()); //deref the deref? wot
    while(it.hasNext())
    {
        it.next();

        //oops idk why i did this (and even implement toColon (which i thought was bugged but was just unreferenced lol)
        //SimplifiedLastModifiedTimestamp timestamp(it.value(), QDateTime::fromMSecsSinceEpoch(it.key()*1000));
        //emit d(SimplifiedLastModifiedTimestamp)
        emit sortedLineOutput(QString::number(it.key()) + " - " + it.value()); //i could make this a colonSeparated line but it would just add confusion etc. maybe in hindsight i should have sorted by that to begin with (but really sorting alphabetically makes more sense when iterating a filesystem (can't [easily(cheaply?)] do sort by timestamp for multiple levels of directories...)
    }
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
