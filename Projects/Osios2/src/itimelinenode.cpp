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
