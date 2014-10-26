#include "timelineserializer.h"

#include <QDataStream>
#include <QSharedPointer>
#include <QBuffer>

#include "osios.h"
#include "itimelinenode.h"
#include "timelinenodetypes/mainmenuactivitychangedtimelinenode.h"
#include "timelinenodetypes/keypressinnewemptydoctimelinenode.h"

TimelineSerializer::TimelineSerializer(QObject *parent)
    : QObject(parent)
{ }
#if 0
bool TimelineSerializer::serializeTimelineToDisk(Osios *osiosContainingTimelineToBeSerialized, QIODevice *ioDeviceToSerializeTo)
{
    QDataStream writeStream(ioDeviceToSerializeTo);
    QList<TimelineNode> timelineNodes = osiosContainingTimelineToBeSerialized->timelineNodes();
    qint32 numTimelineNodes = timelineNodes.size();
    writeStream << numTimelineNodes;
    Q_FOREACH(TimelineNode currentTimelineNode, osiosContainingTimelineToBeSerialized->timelineNodes())
    {
        writeStream << *currentTimelineNode;
    }
    return true;
}
bool TimelineSerializer::deserializeTimelineFromDisk(Osios *osiosContainingTimelineToBeSerialized, QIODevice *ioDeviceToDeserializeFrom)
{
    QDataStream readStream(ioDeviceToDeserializeFrom);
    qint32 numTimelineNodes;
    readStream >> numTimelineNodes;
    for(qint32 i = 0; i < numTimelineNodes; ++i)
    {
        //timeline nodes
        //peek type, instantiate type, read type, append to timeline

        ITimelineNode *instantiatedToDerived = peekInstantiateAndDeserializeNextTimelineNodeFromIoDevice(ioDeviceToDeserializeFrom);

        //append to timeline
        osiosContainingTimelineToBeSerialized->m_TimelineNodes.append(TimelineNode(instantiatedToDerived));
    }
    return true;
}
#endif
ITimelineNode *TimelineSerializer::peekInstantiateAndDeserializeNextTimelineNodeFromIoDevice(QIODevice *ioDeviceToDeserializeFrom)
{
    //peek type
    QByteArray timelineNodeTypePeekByteArray = ioDeviceToDeserializeFrom->peek(sizeof(quint32));
    QBuffer timelineNodeTypePeekBuffer(&timelineNodeTypePeekByteArray);
    timelineNodeTypePeekBuffer.open(QIODevice::ReadOnly);
    QDataStream convertPeekedByteArrayProperlyDataStream(&timelineNodeTypePeekBuffer);
    quint32 timelineNodeTypeId;
    convertPeekedByteArrayProperlyDataStream >> timelineNodeTypeId;

    //instantiate type
    ITimelineNode *instantiatedToDerived = instantiateDerivedTimelineNodeTypeByTypeId(static_cast<TimelineNodeTypeEnum::TimelineNodeTypeEnumActual>(timelineNodeTypeId));

    //read type
    QDataStream readStream(ioDeviceToDeserializeFrom);
    readStream >> *instantiatedToDerived;

    return instantiatedToDerived;
}
ITimelineNode *TimelineSerializer::instantiateDerivedTimelineNodeTypeByTypeId(TimelineNodeTypeEnum::TimelineNodeTypeEnumActual timelineNodeTypeId)
{
    switch(timelineNodeTypeId)
    {
    case TimelineNodeTypeEnum::MainMenuActivityChangedTimelineNode:
        return new MainMenuActivityChangedTimelineNode();
        break;
    case TimelineNodeTypeEnum::KeyPressedInNewEmptyDocTimelineNode:
        return new KeyPressInNewEmptyDocTimelineNode();
        break;
    case TimelineNodeTypeEnum::INITIALNULLINVALIDTIMELINENODETYPE:
    default:
        qFatal("deserialized invalid timeline node type id");
        return 0;
        break;
    }
    return 0;
}
