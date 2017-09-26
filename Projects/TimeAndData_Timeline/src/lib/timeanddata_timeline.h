#ifndef TIMEANDDATA_TIMELINE_H
#define TIMEANDDATA_TIMELINE_H

#include <QObject>

#include <QJsonObject>

#define TimeAndData_Timeline_JSONKEY_TIME "time"
#define TimeAndData_Timeline_JSONKEY_DATA "data"

class TimeAndData_Timeline : public QObject
{
    Q_OBJECT
public:
    explicit TimeAndData_Timeline(QObject *parent = 0);
signals:
    void finishedAppendingJsonObjectToTimeline(bool success);
public slots:
    void appendJsonObjectToTimeline(const QJsonObject &data);
    //TODOprobably: void appendByteArrayToTimeline(const QByteArray &data);
};

#endif // TIMEANDDATA_TIMELINE_H
