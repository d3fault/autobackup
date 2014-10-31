#ifndef ITIMELINENODE_H
#define ITIMELINENODE_H

#include <QDataStream>
#include <QScopedPointer>

class TimelineNodeTypeEnum
{
public:
    //enum is prioritized in semi-order of creation, but the ordering of such enum values in the switch statement in instantiateTimelineNodeBasedOnTimelineNodeType is prioritized based on the programmer's-guess (sup) of which will be the most frequently used
    enum TimelineNodeTypeEnumActual
    {
        INITIALNULLINVALIDTIMELINENODETYPE = 0
        , ProfileCreationAnnounce_aka_GenesisTimelineNode = 1
        , MainMenuActivityChangedTimelineNode = 2
        , KeyPressedInNewEmptyDocTimelineNode = 3
    };
};

class TimelineNodeByteArrayContainsProfileNameEnum //perhaps better description is "serialize for network" and "serialize for disk"
{
public:
    enum TimelineNodeByteArrayContainsProfileNameEnumActual
    {
          TimelineNodeByteArrayDoesContainProfileName = 0 // serializing to/from network
        , TimelineNodeByteArrayDoesNotContainProfileName = 1 // serializing to/from disk (the profile name is the first timeline node of the stream on disk
    };
};

class ITimelineNode;

typedef /*QSharedPointer<*/ITimelineNode*/*>*/ TimelineNode; //is only technically not readwrite when used across multiple threads. Since this app doesn't use threads, I _COULD_ consider it read/write. had this called TimelineNode_ThreadSafeReadOnlyAutoDestructingSharedPointer lol

class ITimelineNode
{
public:
    QByteArray toByteArray(TimelineNodeByteArrayContainsProfileNameEnum::TimelineNodeByteArrayContainsProfileNameEnumActual whetherOrNotToPutTheProfileNameInTheByteArray_YouShouldChooseYesIfNetworkMessageAndNoIfSavingToDisk);
    static ITimelineNode *fromByteArray(/*const */QByteArray &timelineNode, TimelineNodeByteArrayContainsProfileNameEnum::TimelineNodeByteArrayContainsProfileNameEnumActual whetherOrNotTheByteArrayHasProfileNameInIt_IfYouGotItFromNetworkThenYesItDoesButIfFromDiskThenNoItDoesnt);
#if 0
    static TimelineNode makeTimelineNode(ITimelineNode *rawITimelineNodePointer) //only exists for the deleteLater usage
    {
        TimelineNode ret(rawITimelineNodePointer /*, &QObject::deleteLater*/);
        return ret;
    }
#endif
    explicit ITimelineNode();
    explicit ITimelineNode(TimelineNodeTypeEnum::TimelineNodeTypeEnumActual timelineNodeType, qint64 UnixTimestamp_OrZeroToUseCurrentTime);
    explicit ITimelineNode(const ITimelineNode &other);
    virtual ~ITimelineNode();

    QString ProfileName; //sent across network with each timeline node, but only ever serialized once (first timeline node serialized)

    TimelineNodeTypeEnum::TimelineNodeTypeEnumActual TimelineNodeType;
    qint64 UnixTimestamp;

    virtual QString humanReadableShortDescription() const=0;
    QString humanReadableShortDescriptionIncludingTimestamp() const;
protected:
    virtual QDataStream &save(QDataStream &outputStream) const=0;
    virtual QDataStream &load(QDataStream &inputStream)=0;
private:
    friend QDataStream &operator<<(QDataStream &outputStream, const ITimelineNode &timelineNode);
    friend QDataStream &operator>>(QDataStream &inputStream, ITimelineNode &timelineNode);
};

#if 0
QDataStream &operator<< (QDataStream &outputStream, const ITimelineNode &timelineNode);
QDataStream &operator>> (QDataStream &inputStream, ITimelineNode &timelineNode);
#endif

Q_DECLARE_METATYPE(TimelineNode)

#if 0 //I picked a hell of a time to try to learn implicit sharing with polymorphism
#include <QSharedData>
#include <QString>
#include <QDataStream>

//#define AbstractTimelineNodeTemplateMacro template<typename T = ITimelineNode>
#define AbstractTimelineNode template<typename T = ITimelineNodeData>
//gg


class ITimelineNodeData : public QSharedData
{
public:
    ITimelineNodeData()
        : TimelineNodeType(TimelineNodeTypeEnum::INITIALNULLINVALIDTIMELINENODETYPE)
    { }
    ITimelineNodeData(TimelineNodeTypeEnum::TimelineNodeTypeEnumActual timelineNodeType)
        : TimelineNodeType(timelineNodeType)
    { }
    ITimelineNodeData(const ITimelineNodeData &other)
        : QSharedData(other)
        , TimelineNodeType(other.TimelineNodeType)
    { }
    virtual ~ITimelineNodeData();

    QString humanReadableShortDescription() const;

    virtual QDataStream &save(QDataStream &outputStream) const;
    virtual QDataStream &load(QDataStream &inputStream);

    int TimelineNodeType;
};

AbstractTimelineNode
class ITimelineNode
{
    public:
    ITimelineNode() { d = new ITimelineNodeData; }
    ITimelineNode(TimelineNodeTypeEnum::TimelineNodeTypeEnumActual timelineNodeType) { d = new ITimelineNodeData(timelineNodeType); }
    ITimelineNode(const ITimelineNode &other)
        : d (other.d)
    { }
    virtual ~ITimelineNode();

    QString humanReadableShortDescription() const;

    void setTimelineNodeType(int timelineNodeType) { d->TimelineNodeType = timelineNodeType; }
    int timelineNodeType() const { return d->TimelineNodeType; }
    private:
    friend QDataStream &operator<< (QDataStream &outputStream, const ITimelineNode &timelineNode);
    friend QDataStream &operator>> (QDataStream &inputStream, ITimelineNode &timelineNode);
    QSharedDataPointer<T> d;
};

template<typename T>
QDataStream &operator<< (QDataStream &outputStream, const ITimelineNode<T> &timelineNode);
template<typename T>
QDataStream &operator>> (QDataStream &inputStream, ITimelineNode<T> &timelineNode);

//Q_DECLARE_METATYPE(ITimelineNode<T>)
#endif

#endif // ITIMELINENODE_H
