void UserInterfaceSkeletonGenerator::populateDataUsingHardCodedCppXD(Data &data)
{
    data.BusinessLogicClassName = "TimeAndData_Timeline";

    data.createAndAddSignal("e", ArgsWithOptionalDefaultValues_List() << SingleArgWithOptionalDefaultValue("const QString &","msg"));

    data.createAndAddSignal("timelineEntryRead", ArgsWithOptionalDefaultValues_List() << SingleArgWithOptionalDefaultValue("const TimeAndDataAndParentId_TimelineEntry &","timelineEntry"));

    data.createAndAddRequestResponse_aka_SlotWithFinishedSignal("readAndEmitAllTimelineEntriesInInLessEfficientForwardsChronologicalOrder", ArgsList(), ArgsWithMandatoryDefaultValues_List() << SingleArgWithMandatoryDefaultValue("TimelineEntryIdType","latestTimelineEntryId","TimelineEntryIdType()"));

    data.createAndAddRequestResponse_aka_SlotWithFinishedSignal("appendJsonObjectToTimeline", ArgsList() << SingleArg("const QJsonObject &", "data"), ArgsWithMandatoryDefaultValues_List() << SingleArgWithMandatoryDefaultValue("TimeAndDataAndParentId_TimelineEntry", "timelineEntry", "TimeAndDataAndParentId_TimelineEntry()"));
}
