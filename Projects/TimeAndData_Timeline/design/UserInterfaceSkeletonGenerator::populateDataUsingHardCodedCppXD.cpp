void UserInterfaceSkeletonGenerator::populateDataUsingHardCodedCppXD(Data &data)
{
    data.BusinessLogicClassName = "TimeAndData_Timeline";

    data.createAndAddSignal("e", ArgsWithOptionalDefaultValues_List() << SingleArgWithOptionalDefaultValue("const QString &","msg"));

    data.createAndAddRequestResponse_aka_SlotWithFinishedSignal("readAndEmitAllTimelineEntries", ArgsList(), ArgsWithMandatoryDefaultValues_List() << SingleArgWithMandatoryDefaultValue("AllTimelineEntriesType","allTimelineEntries","AllTimelineEntriesType()"));

    data.createAndAddRequestResponse_aka_SlotWithFinishedSignal("appendJsonObjectToTimeline", ArgsList() << SingleArg("const QJsonObject &", "data"));
}
