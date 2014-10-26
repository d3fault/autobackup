#include "itimelinenode.h"

#include <QString>

QDataStream &operator<<(QDataStream &outputStream, const ITimelineNode &timelineNode)
{
    return timelineNode.save(outputStream);
}
QDataStream &operator>>(QDataStream &inputStream, ITimelineNode &timelineNode)
{
    return timelineNode.load(inputStream);
}
#if 0
ITimelineNodeData::~ITimelineNodeData()
{ }
QString ITimelineNodeData::humanReadableShortDescription() const
{
    //derived classes must override this
    //return "Invalid Timeline Node";
    return QString::number(TimelineNodeType);
}
ITimelineNode::~ITimelineNode()
{ }
#endif
ITimelineNode::ITimelineNode()
    : TimelineNodeType(TimelineNodeTypeEnum::INITIALNULLINVALIDTIMELINENODETYPE)
{ }
ITimelineNode::ITimelineNode(TimelineNodeTypeEnum::TimelineNodeTypeEnumActual timelineNodeType)
    : TimelineNodeType(timelineNodeType)
{ }
ITimelineNode::ITimelineNode(const ITimelineNode &other)
    : TimelineNodeType(other.TimelineNodeType)
{ }
ITimelineNode::~ITimelineNode()
{ }
QString ITimelineNode::humanReadableShortDescription() const
{
    //derived classes should override this (i think, but could be wrong). since my d is private and... not sure if it will resolve to the derived d or what
    return "Unknown or Invalid Timeline Node Short Description with TypeId of: " + QString::number(TimelineNodeType); //TODOoptional: convert to human readable type name from type id
    //return d->humanReadableShortDescription();
}
QDataStream &ITimelineNode::save(QDataStream &outputStream) const
{
    //derived classes must override this to serialize their data, but they must call their parent class first
    outputStream << static_cast<quint32>(TimelineNodeType);
    return outputStream;
}
QDataStream &ITimelineNode::load(QDataStream &inputStream)
{
    //derived classes must override this to serialize their data, but they should call must parent class first
    quint32 timelineNodeTypeData; //quint32 should keep be busy for a while
    inputStream >> timelineNodeTypeData;
    TimelineNodeType = static_cast<TimelineNodeTypeEnum::TimelineNodeTypeEnumActual>(timelineNodeTypeData);
    return inputStream;
}
