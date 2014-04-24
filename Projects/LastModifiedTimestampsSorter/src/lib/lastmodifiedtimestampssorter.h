#ifndef LASTMODIFIEDTIMESTAMPSSORTER_H
#define LASTMODIFIEDTIMESTAMPSSORTER_H

#include <QObject>
#include <QIODevice>
#include <QFile>

class LastModifiedTimestampsSorter : public QObject
{
    Q_OBJECT
public:
    explicit LastModifiedTimestampsSorter(QObject *parent = 0);
private:
    void formatAndOutputSortedLine(long long timestamp, const QString &path);
signals:
    void d(const QString &);
    void sortedLineOutput(const QString &);
    void finishedOutputtingSortedLines();
public slots:
    void sortAndOutputLastModifiedTimestamps(QIODevice *lastModifiedTimestampsIoDevice); //TODOoptional: specify sorting type
    void sortAndOutputLastModifiedTimestamps(const QString &lastModifiedTimestampsFilename);
};

#endif // LASTMODIFIEDTIMESTAMPSSORTER_H
