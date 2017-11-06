void UserInterfaceSkeletonGenerator::populateDataUsingHardCodedCppXD(Data &data)
{
    data.BusinessLogicClassName = "SocialNetworkTimeline";

    data.createAndAddSignal("e", ArgsWithOptionalDefaultValues_List() << SingleArgWithOptionalDefaultValue("QString","msg"));
    data.createAndAddSignal("o", ArgsWithOptionalDefaultValues_List() << SingleArgWithOptionalDefaultValue("QString","msg"));

    data.createAndAddRequestResponse_aka_SlotWithFinishedSignal("initializeSocialNetworkTimeline");

    data.createAndAddRequestResponse_aka_SlotWithFinishedSignal("appendJsonObjectToSocialNetworkTimeline", ArgsList() << SingleArg("const QJsonObject &", "data"));
}
