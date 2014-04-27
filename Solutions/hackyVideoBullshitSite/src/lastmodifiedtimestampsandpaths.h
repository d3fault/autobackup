#ifndef LASTMODIFIEDTIMESTAMPSANDPATHS_H
#define LASTMODIFIEDTIMESTAMPSANDPATHS_H

#include <boost/unordered_map.hpp>

#include <QList>
#include <QListIterator>
#include <QHash>
#include <QByteArray>

typedef boost::unordered_map<std::string /*path*/, int /*index into sorted flat list*/> PathsIndexIntoFlatListHashType;

struct TimestampAndPath
{
    TimestampAndPath(long long timestamp, std::string path)
        : Timestamp(timestamp), Path(path)
    { }
    long long Timestamp;
    std::string Path;
};

struct LastModifiedTimestampsAndPaths
{
    LastModifiedTimestampsAndPaths(QList<TimestampAndPath*> *sortedTimestampAndPathFlatList, PathsIndexIntoFlatListHashType *pathsIndexIntoFlatListHash)
        : SortedTimestampAndPathFlatList(sortedTimestampAndPathFlatList), PathsIndexIntoFlatListHash(pathsIndexIntoFlatListHash)
    { }
    ~LastModifiedTimestampsAndPaths()
    {
        QListIterator<TimestampAndPath*> sortedTimestampAndPathFlatListIterator(*SortedTimestampAndPathFlatList);
        while(sortedTimestampAndPathFlatListIterator.hasNext())
        {
            TimestampAndPath *toDelete = sortedTimestampAndPathFlatListIterator.next();
            delete toDelete;
        }
        delete SortedTimestampAndPathFlatList;
        delete PathsIndexIntoFlatListHash;
    }
    QList<TimestampAndPath*> *SortedTimestampAndPathFlatList;
    PathsIndexIntoFlatListHashType *PathsIndexIntoFlatListHash;
    //qhash doesn't like std::string :(. QHash<std::string, int> *PathsIndexIntoFlatListHash; //TODOoptimization: a bimap would probably save memory, and might even be faster
};


#endif // LASTMODIFIEDTIMESTAMPSANDPATHS_H
