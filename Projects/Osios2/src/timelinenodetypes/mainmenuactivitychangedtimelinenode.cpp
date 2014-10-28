#include "mainmenuactivitychangedtimelinenode.h"

//increase if more tabs are added
#define OsiosMainMenuSerializedDataType quint8

#if 0
MainMenuActivityChangedTimelineNodeData::~MainMenuActivityChangedTimelineNodeData()
{ }
#endif
MainMenuActivityChangedTimelineNode::~MainMenuActivityChangedTimelineNode()
{ }
QString MainMenuActivityChangedTimelineNode::humanReadableShortDescription() const
{
    return "Main Menu Activity Changed To: " + QString::number(NewMainMenuActivity); //TODOoptional: human readable instead of enum int value
}
QDataStream &MainMenuActivityChangedTimelineNode::save(QDataStream &outputStream) const
{
    outputStream << static_cast<OsiosMainMenuSerializedDataType>(NewMainMenuActivity);
    return outputStream;
}
QDataStream &MainMenuActivityChangedTimelineNode::load(QDataStream &inputStream)
{
    OsiosMainMenuSerializedDataType newMainMenuActivityData;
    inputStream >> newMainMenuActivityData;
    NewMainMenuActivity = static_cast<MainMenuActivitiesEnum::MainMenuActivitiesEnumActual>(newMainMenuActivityData);
    return inputStream;
}
