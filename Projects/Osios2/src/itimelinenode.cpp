#include "itimelinenode.h"

#include <QString>
#include <QBuffer>

//I could code a checksum (even a cryptographic one) into the timeline node protocol but I won't since the receiving neighbors of the data cryptographically hash it and send their result of that computation back to the sender. Sending the checksum is then worthless. TODOreq: don't "apply" a timeline node until the sender confirms that your computed checksum is good (we trust tcp to deliver that 'your checksum is good' message (gasp maybe I need a checksum after all))
//HOWEVER a serialized checksum would maybe be useful when doing long-term periodic checks of data. Allows you to distribute the load/check, whereas if you have to verify the entire tree [up to a point] to verify a file, it's kind of expensive and not distributed. For now I'm going to ignore the checksumming-in-any-protocol (network vs serialized vs both vs neither) considerations altogether and only have neighbor cryptographic hash verification
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
QByteArray ITimelineNode::toByteArray()
{
    QByteArray timelineNodeRawByteArray;

    {
        QBuffer timelineNodeRawBuffer(&timelineNodeRawByteArray);
        timelineNodeRawBuffer.open(QIODevice::WriteOnly);
        QDataStream timelineNodeSerializer(&timelineNodeRawBuffer);
        timelineNodeSerializer << *this;
    }

    return timelineNodeRawByteArray;
}
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
