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

    //sync and take
    SortedMapOfListsOfPathsPointerType *sortLastModifiedTimestamps_ButDontSortPaths(QIODevice *lastModifiedTimestampsIoDevice, int *itemsCount = 0);
    SortedMapOfListsOfPathsPointerType *sortLastModifiedTimestamps_ButDontSortPaths(const QString &lastModifiedTimestampsFilename, int *itemsCount = 0);

    //sync and take
    SortedMapOfListsOfPathsPointerType *sortLastModifiedTimestamps(QIODevice *lastModifiedTimestampsIoDevice, int *itemsCount = 0);
    SortedMapOfListsOfPathsPointerType *sortLastModifiedTimestamps(const QString &lastModifiedTimestampsFilename, int *itemsCount = 0);

    ~LastModifiedTimestampsSorter();
private:
    SortedMapOfListsOfPathsPointerType *m_MapOfPathsListsSortedByModificationDate;
    int m_TotalItemsCount;

    void newTheMapIfNeeded();
    void clearTheMap();
    void sortAllLastModifiedTimestampsKeysAkaTimestampsOnlyOnIoDevice(QIODevice *lastModifiedTimestampsIoDevice);
    void sortAllLastModifiedTimestampsValuesAkaPathsOnInternalMap();
    void outputTheMap();
signals:
    void d(const QString &);

    //async
    void lastModifiedTimestampsSorted(SortedMapOfListsOfPathsPointerType*, int); //signal receivers take owenrship, i guess this is a good place for implicit sharing (i'm doing it wrong) xD. but then which of those receivers deletes the map's values ;-)??

    //sync
    void sortedLineOutput(const QString &);
    void finishedOutputtingSortedLines(int totalItemsCount);
public slots:
    //async
    void sortAndEmitLastModifiedTimestamps(QIODevice *lastModifiedTimestampsIoDevice);
    void sortAndEmitLastModifiedTimestamps(const QString &lastModifiedTimestampsFilename);

    //sync
    void sortAndOutputLastModifiedTimestamps(QIODevice *lastModifiedTimestampsIoDevice); //TODOoptional: specify sorting type
    void sortAndOutputLastModifiedTimestamps(const QString &lastModifiedTimestampsFilename);
};

#endif // LASTMODIFIEDTIMESTAMPSSORTER_H
