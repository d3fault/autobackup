#include "mainmenuactivitychangedtimelinenode.h"

//increase if more tabs are added
#define OsiosMainMenuSerializedDataType quint8

MainMenuActivityChangedTimelineNodeData::~MainMenuActivityChangedTimelineNodeData()
{ }
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
