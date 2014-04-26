#ifndef LASTMODIFIEDTIMESTAMPSSORTER_H
#define LASTMODIFIEDTIMESTAMPSSORTER_H

#include <QObject>
#include <QMap>
#include <QList>

class QIODevice;

typedef QMap<long long, QList<std::string>* > SortedMapOfListsOfPathsPointerType;

class LastModifiedTimestampsSorter : public QObject
{
    Q_OBJECT
public:
    explicit LastModifiedTimestampsSorter(QObject *parent = 0);
    SortedMapOfListsOfPathsPointerType *takeMapOfPathsListsSortedByModificationDate();
    ~LastModifiedTimestampsSorter();
private:
    SortedMapOfListsOfPathsPointerType *m_MapOfPathsListsSortedByModificationDate;

    void newTheMapIfNeeded();
    void clearTheMapIfNeeded();
    void sortAllLastModifiedTimestampsOnIoDevice(QIODevice *lastModifiedTimestampsIoDevice);
    void outputTheMap();
signals:
    void d(const QString &);

    //async
    void lastModifiedTimestampsSorted(SortedMapOfListsOfPathsPointerType*); //signal receivers take owenrship, i guess this is a good place for implicit sharing (i'm doing it wrong) xD. but then which of those receivers deletes the map's values ;-)??

    //sync
    void sortedLineOutput(const QString &);
    void finishedOutputtingSortedLines();
public slots:
    //async
    void sortAndEmitLastModifiedTimestamps(QIODevice *lastModifiedTimestampsIoDevice);
    void sortAndEmitLastModifiedTimestamps(const QString &lastModifiedTimestampsFilename);

    //sync
    void sortAndOutputLastModifiedTimestamps(QIODevice *lastModifiedTimestampsIoDevice); //TODOoptional: specify sorting type
    void sortAndOutputLastModifiedTimestamps(const QString &lastModifiedTimestampsFilename);
};

#endif // LASTMODIFIEDTIMESTAMPSSORTER_H
