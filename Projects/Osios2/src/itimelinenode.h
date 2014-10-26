#ifndef ITIMELINENODE_H
#define ITIMELINENODE_H

#include <QSharedData>
#include <QString>
#include <QDataStream>

class TimelineNodeTypeEnum
{
public:
    enum TimelineNodeTypeEnumActual
    {
          INITIALNULLINVALIDTIMELINENODETYPE = 0
        , MainMenuActivityChangedTimelineNode = 1
        , KeyPressedInNewEmptyDocTimelineNode = 2
    };
};

class ITimelineNodeDataITimelineNodeData : public QSharedData
{
  public:
    ITimelineNodeDataITimelineNodeData()
        : TimelineNodeType(TimelineNodeTypeEnum::INITIALNULLINVALIDTIMELINENODETYPE)
    { }
    ITimelineNodeDataITimelineNodeData(TimelineNodeTypeEnum::TimelineNodeTypeEnumActual timelineNodeType)
        : TimelineNodeType(timelineNodeType)
    { }
    ITimelineNodeDataITimelineNodeData(const ITimelineNodeDataITimelineNodeData &other)
        : QSharedData(other)
        , TimelineNodeType(other.TimelineNodeType)
    { }
    virtual ~ITimelineNodeDataITimelineNodeData();

    QString humanReadableShortDescription() const;

    virtual QDataStream &save(QDataStream &outputStream) const;
    virtual QDataStream &load(QDataStream &inputStream);

    int TimelineNodeType;
};

class ITimelineNode
{
  public:
    ITimelineNode() { d = new ITimelineNodeDataITimelineNodeData; }
    ITimelineNode(TimelineNodeTypeEnum::TimelineNodeTypeEnumActual timelineNodeType) { d = new ITimelineNodeDataITimelineNodeData(timelineNodeType); }
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
    QSharedDataPointer<ITimelineNodeDataITimelineNodeData> d;
};

QDataStream &operator<< (QDataStream &outputStream, const ITimelineNode &timelineNode);
QDataStream &operator>> (QDataStream &inputStream, ITimelineNode &timelineNode);

Q_DECLARE_METATYPE(ITimelineNode)

#endif // ITIMELINENODE_H
