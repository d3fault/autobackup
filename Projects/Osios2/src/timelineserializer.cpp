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
#if 0 //even though i didn't CALL either of these two methods from anywhere, somehow writing them and getting them to work fixed the other similar code xD
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
ITimelineNode *TimelineSerializer::peekInstantiateAndDeserializeNextTimelineNodeFromIoDevice(QIODevice *ioDeviceToDeserializeFrom, TimelineNodeByteArrayContainsProfileNameEnum::TimelineNodeByteArrayContainsProfileNameEnumActual whetherOrNotTheByteArrayHasProfileNameInIt_IfYouGotItFromNetworkThenYesItDoesButIfFromDiskThenNoItDoesnt)
{
    QDataStream readStream(ioDeviceToDeserializeFrom);
    QString profileNameMaybe;
    if(whetherOrNotTheByteArrayHasProfileNameInIt_IfYouGotItFromNetworkThenYesItDoesButIfFromDiskThenNoItDoesnt == TimelineNodeByteArrayContainsProfileNameEnum::TimelineNodeByteArrayDoesContainProfileName)
    {
        //profile name comes just before timeline node type (if it's there at all (only exists in network variants))
        readStream >> profileNameMaybe;
        if(profileNameMaybe.trimmed().isEmpty())
        {
            qDebug("profile name empty on timeline node received over network, wtf?"); //TODOreq: use notification system
        }
    }

    //peek type
    QByteArray timelineNodeHeaderPeekByteArray = ioDeviceToDeserializeFrom->peek(sizeof(quint32) /*timeline node type*/);
    QBuffer timelineNodeHeaderPeekBuffer(&timelineNodeHeaderPeekByteArray);
    timelineNodeHeaderPeekBuffer.open(QIODevice::ReadOnly);
    QDataStream convertPeekedByteArrayProperlyDataStream(&timelineNodeHeaderPeekBuffer);
    quint32 timelineNodeType;
    convertPeekedByteArrayProperlyDataStream >> timelineNodeType; //timeline node type

    //instantiate type
    ITimelineNode *instantiatedToDerived = instantiateTimelineNodeBasedOnTimelineNodeType(static_cast<TimelineNodeTypeEnum::TimelineNodeTypeEnumActual>(timelineNodeType));

    //read type

    readStream >> *instantiatedToDerived;

    if(!profileNameMaybe.isEmpty())
    {
        instantiatedToDerived->ProfileName = profileNameMaybe;
    }

    return instantiatedToDerived;
}
ITimelineNode *TimelineSerializer::instantiateTimelineNodeBasedOnTimelineNodeType(TimelineNodeTypeEnum::TimelineNodeTypeEnumActual timelineNodeTypeId)
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
        QString timelineNodeIdString = QString::number(static_cast<quint32>(timelineNodeTypeId));
        qFatal(std::string("deserialized invalid timeline node type id: " + timelineNodeIdString.toStdString()).c_str()); //TODOmb: quit and release resources etc? idk how exactly qFatal works..
        return 0;
        break;
    }
    return 0;
}
