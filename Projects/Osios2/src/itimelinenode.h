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
    void setTimelineNodeType(int timelineNodeType) { d->TimelineNodeType = timelineNodeType; }
    int timelineNodeType() const { return d->TimelineNodeType; }
  private:
    friend QDataStream &operator<< (QDataStream &outputStream, const ITimelineNode &timelineNode);
    friend QDataStream &operator>> (QDataStream &inputStream, ITimelineNode &timelineNode);
    QSharedDataPointer<ITimelineNodeDataITimelineNodeData> d;
};

QDataStream &operator<< (QDataStream &outputStream, const ITimelineNode &timelineNode);
QDataStream &operator>> (QDataStream &inputStream, ITimelineNode &timelineNode);

#endif // ITIMELINENODE_H
