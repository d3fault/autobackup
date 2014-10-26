#include "itimelinenode.h"

QDataStream &operator<<(QDataStream &outputStream, const ITimelineNode &timelineNode)
{
    return timelineNode.d->save(outputStream);
}
QDataStream &operator>>(QDataStream &inputStream, ITimelineNode &timelineNode)
{
    return timelineNode.d->load(inputStream);
}
ITimelineNodeDataITimelineNodeData::~ITimelineNodeDataITimelineNodeData()
{ }
QString ITimelineNodeDataITimelineNodeData::humanReadableShortDescription() const
{
    //derived classes must override this
    //return "Invalid Timeline Node";
    return QString::number(TimelineNodeType); //TODOoptional: convert to human readable type
}
QDataStream &ITimelineNodeDataITimelineNodeData::save(QDataStream &outputStream) const
{
    //derived classes must override this to serialize their data, but they should call their parent class first
    outputStream << static_cast<quint32>(TimelineNodeType);
    return outputStream;
}
QDataStream &ITimelineNodeDataITimelineNodeData::load(QDataStream &inputStream)
{
    //derived classes must override this to serialize their data, but they should call their parent class first
    quint32 timelineNodeTypeData; //quint32 should keep be busy for a while
    inputStream >> timelineNodeTypeData;
    TimelineNodeType = static_cast<TimelineNodeTypeEnum::TimelineNodeTypeEnumActual>(timelineNodeTypeData);
    return inputStream;
}
ITimelineNode::~ITimelineNode()
{ }
QString ITimelineNode::humanReadableShortDescription() const
{
    //derived classes must override this (i think, but could be wrong). since my d is private and... not sure if it will resolve to the derived d or what
    return d->humanReadableShortDescription();
}
