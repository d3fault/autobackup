#ifndef TIMEANDDATA_TIMELINE_H
#define TIMEANDDATA_TIMELINE_H

#include <QObject>

#include <QJsonObject>
#include <QMultiMap>

#define TimeAndData_Timeline_JSONKEY_TIME "time"
#define TimeAndData_Timeline_JSONKEY_DATA "data"

typedef QMultiMap<qint64 /*time*/, QJsonObject /*data*/> AllTimelineEntriesType;

class TimeAndData_Timeline : public QObject
{
    Q_OBJECT
public:
    explicit TimeAndData_Timeline(QObject *parent = 0);
signals:
    void finishedReadingAllTimelineEntries(const AllTimelineEntriesType &allTimelineEntries);
    void finishedAppendingJsonObjectToTimeline(bool success);
public slots:
    void readAndEmitAllTimelineEntries();
    void appendJsonObjectToTimeline(const QJsonObject &data);
    //TODOprobably: void appendByteArrayToTimeline(const QByteArray &data);
};

#endif // TIMEANDDATA_TIMELINE_H
