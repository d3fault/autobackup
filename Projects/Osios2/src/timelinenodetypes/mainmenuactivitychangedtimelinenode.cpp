#include "mainmenuactivitychangedtimelinenode.h"

//increase if more tabs are added
#define OsiosMainMenuSerializedDataType quint8

MainMenuActivityChangedTimelineNodeData::~MainMenuActivityChangedTimelineNodeData()
{ }
#if 0
QString MainMenuActivityChangedTimelineNodeData::humanReadableShortDescription() const
{
    return "Main Menu Activity Changed To: " + QString::number(NewMainMenuActivity); //TODOoptional: human readable instead of enum int value
}
#endif
QDataStream &MainMenuActivityChangedTimelineNodeData::save(QDataStream &outputStream) const
{
    ITimelineNodeDataITimelineNodeData::save(outputStream);
    outputStream << static_cast<OsiosMainMenuSerializedDataType>(NewMainMenuActivity);
    return outputStream;
}
QDataStream &MainMenuActivityChangedTimelineNodeData::load(QDataStream &inputStream)
{
    ITimelineNodeDataITimelineNodeData::load(inputStream);
    OsiosMainMenuSerializedDataType newMainMenuActivityData;
    inputStream >> newMainMenuActivityData;
    NewMainMenuActivity = static_cast<MainMenuActivitiesEnum::MainMenuActivitiesEnumActual>(newMainMenuActivityData);
    return inputStream;
}
MainMenuActivityChangedTimelineNode::~MainMenuActivityChangedTimelineNode()
{ }
