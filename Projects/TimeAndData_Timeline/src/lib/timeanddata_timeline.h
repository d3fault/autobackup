#ifndef TIMEANDDATA_TIMELINE_H
#define TIMEANDDATA_TIMELINE_H

#include <QObject>

#include <QJsonObject>
#include <QMultiMap>
#include <QScopedPointer>

#define TimeAndData_Timeline_JSONKEY_TIME "time"
#define TimeAndData_Timeline_JSONKEY_DATA "data"

typedef QMultiMap<qint64 /*time*/, QJsonObject /*data*/> AllTimelineEntriesType;

namespace TimeAndData_TimelineRequestResponseContracts
{
    class Contracts;
}

class TimeAndData_Timeline : public QObject
{
    Q_OBJECT
public:
    template<class T>
    static void establishConnectionsToAndFromBackendAndUi(TimeAndData_Timeline *backend, T *ui)
    {
        connect(ui, &T::readAndEmitAllTimelineEntriesRequested, backend, &TimeAndData_Timeline::readAndEmitAllTimelineEntries);
        connect(ui, &T::appendJsonObjectToTimelineRequested, backend, &TimeAndData_Timeline::appendJsonObjectToTimeline);
        connect(backend, &TimeAndData_Timeline::e, ui, &T::handleE);
        connect(backend, &TimeAndData_Timeline::readAndEmitAllTimelineEntriesFinished, ui, &T::handleReadAndEmitAllTimelineEntriesFinished);
        connect(backend, &TimeAndData_Timeline::appendJsonObjectToTimelineFinished, ui, &T::handleAppendJsonObjectToTimelineFinished);
    }

    explicit TimeAndData_Timeline(QObject *parent = 0);
    ~TimeAndData_Timeline();
private:
    QScopedPointer<TimeAndData_TimelineRequestResponseContracts::Contracts> m_Contracts;
signals:
    void e(const QString &msg);
    void readAndEmitAllTimelineEntriesFinished(bool success, const AllTimelineEntriesType &allTimelineEntries);
    void appendJsonObjectToTimelineFinished(bool success);
public slots:
    void readAndEmitAllTimelineEntries();
    void appendJsonObjectToTimeline(const QJsonObject &data);
    //TODOprobably: void appendByteArrayToTimeline(const QByteArray &data);
};

#endif // TIMEANDDATA_TIMELINE_H
