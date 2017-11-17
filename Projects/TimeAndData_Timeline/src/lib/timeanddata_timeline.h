#ifndef TIMEANDDATA_TIMELINE_H
#define TIMEANDDATA_TIMELINE_H

#include <QObject>

#include <memory>

#include <QJsonObject>
#include <QScopedPointer>

//per timeline-entry
#define TimeAndData_Timeline_JSONKEY_TIME "time"
#define TimeAndData_Timeline_JSONKEY_DATA "data"
#define TimeAndData_Timeline_JSONKEY_PARENT_TIMELINE_ENTRY_ID "parentTimelineEntryId"

//per timeline
#define TimeAndData_Timeline_SETTINGSGROUP_TIMELINE_ENTRIES "timelineEntries"
#define TimeAndData_Timeline_SETTINGSKEY_LATEST_TIMELINE_ENTRY_ID_AKA_HEAD_IN_GIT_LINGO "latestTimelineEntryId"

typedef QString TimelineEntryIdType;
class TimeAndDataAndParentId_TimelineEntry// : public QJsonObject
{
public:
    TimeAndDataAndParentId_TimelineEntry()=default;
    //TimeAndDataAndParentId_TimelineEntry(const QJsonDataObject &other)=delete;
    TimeAndDataAndParentId_TimelineEntry(const TimeAndDataAndParentId_TimelineEntry &other)=default;
    TimeAndDataAndParentId_TimelineEntry(qint64 time, const QJsonObject &data, TimelineEntryIdType parentTimelineEntryId)
        : m_Time(time)
        , m_Data(data)
        , m_ParentTimelineEntryId(parentTimelineEntryId)
    { }
    QJsonObject toJsonObject() const;
    //static TimeAndDataAndParentId_TimelineEntry fromJsonObject(const QJsonObject &jsonObject);
    TimelineEntryIdType timelineEntryId();
    //QByteArray toJson() const;

    qint64 time() const;
    void setTime(const qint64 &newTime);

    QJsonObject data() const;
    void setData(const QJsonObject &newData);

    TimelineEntryIdType parentTimelineEntryId() const;
    void setParentTimelineEntryId(const TimelineEntryIdType &newParentTimelineEntryId);
private:
    TimelineEntryIdType privateGenerateTimelineEntryId() const;

    qint64 m_Time = 0;
    QJsonObject m_Data;
    TimelineEntryIdType m_ParentTimelineEntryId;

    TimelineEntryIdType m_CachedTimelineEntryId;
    bool m_CachedTimelineEntryId_IsStale = true;
};

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
        connect(ui, &T::readAndEmitAllTimelineEntriesInInLessEfficientForwardsChronologicalOrderRequested, backend, &TimeAndData_Timeline::readAndEmitAllTimelineEntriesInInLessEfficientForwardsChronologicalOrder);
        connect(ui, &T::appendJsonObjectToTimelineRequested, backend, &TimeAndData_Timeline::appendJsonObjectToTimeline);
        connect(backend, &TimeAndData_Timeline::e, ui, &T::handleE);
        connect(backend, &TimeAndData_Timeline::timelineEntryRead, ui, &T::handleTimelineEntryRead);
        connect(backend, &TimeAndData_Timeline::readAndEmitAllTimelineEntriesInInLessEfficientForwardsChronologicalOrderFinished, ui, &T::handleReadAndEmitAllTimelineEntriesInInLessEfficientForwardsChronologicalOrderFinished);
        connect(backend, &TimeAndData_Timeline::appendJsonObjectToTimelineFinished, ui, &T::handleAppendJsonObjectToTimelineFinished);
    }

    explicit TimeAndData_Timeline(QObject *parent = 0);
    ~TimeAndData_Timeline();
private:
    //static QString quickHashEncoded(const QByteArray &input);
    bool parseJsonString(const QString &jsonInputString, QJsonObject *out_JsonObject);
    bool populateTimelineEntryFromJsonObject(const QJsonObject &timeAndDataAndParentIdJson, TimeAndDataAndParentId_TimelineEntry &out_TimelineEntry);
    TimelineEntryIdType nullAkaEmptyAkaRootTimelineEntryId() const;

    QScopedPointer<TimeAndData_TimelineRequestResponseContracts::Contracts> m_Contracts;
signals:
    void e(const QString &msg);
    void timelineEntryRead(const TimeAndDataAndParentId_TimelineEntry &timelineEntry);
    void readAndEmitAllTimelineEntriesInInLessEfficientForwardsChronologicalOrderFinished(bool success, const TimelineEntryIdType &latestTimelineEntryId);
    void appendJsonObjectToTimelineFinished(bool success, const TimeAndDataAndParentId_TimelineEntry &timelineEntry);
public slots:
    void readAndEmitAllTimelineEntriesInInLessEfficientForwardsChronologicalOrder();
    void appendJsonObjectToTimeline(const QJsonObject &data);
    //TODOprobably: void appendByteArrayToTimeline(const QByteArray &data);
};

#endif // TIMEANDDATA_TIMELINE_H
