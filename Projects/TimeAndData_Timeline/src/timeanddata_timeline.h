#ifndef TIMEANDDATA_TIMELINE_H
#define TIMEANDDATA_TIMELINE_H

#include <QObject>

#include <QJsonObject>

class TimeAndData_Timeline : public QObject
{
    Q_OBJECT
public:
    explicit TimeAndData_Timeline(QObject *parent = 0);
public slots:
    void appendJsonObjectToTimeline(const QJsonObject &data);
    //TODOprobably: void appendByteArrayToTimeline(const QByteArray &data);
};

#endif // TIMEANDDATA_TIMELINE_H
