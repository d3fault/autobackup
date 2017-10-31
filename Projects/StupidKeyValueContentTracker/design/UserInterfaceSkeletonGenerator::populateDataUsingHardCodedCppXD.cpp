void UserInterfaceSkeletonGenerator::populateDataUsingHardCodedCppXD(Data &data)
{
    data.BusinessLogicClassName = "StupidKeyValueContentTracker";

    data.createAndAddSignal("e", ArgsWithOptionalDefaultValues_List() << SingleArgWithOptionalDefaultValue("QString","msg"));
    data.createAndAddSignal("o", ArgsWithOptionalDefaultValues_List() << SingleArgWithOptionalDefaultValue("QString","msg"));

    data.createAndAddRequestResponse_aka_SlotWithFinishedSignal("initialize");

    data.createAndAddRequestResponse_aka_SlotWithFinishedSignal("add", ArgsList() << SingleArg("const QString &", "key") << SingleArg("const QString &", "data"));

    data.createAndAddRequestResponse_aka_SlotWithFinishedSignal("modify", ArgsList() << SingleArg("const QString &", "key") << SingleArg("const QString &", "newValue"));

    data.createAndAddRequestResponse_aka_SlotWithFinishedSignal("removeKey", ArgsList() << SingleArg("const QString &", "key"));

    data.createAndAddRequestResponse_aka_SlotWithFinishedSignal("commit", ArgsList() << SingleArg("const QString &", "commitMessage"));

    data.createAndAddRequestResponse_aka_SlotWithFinishedSignal("readKey", ArgsList() << SingleArg("const QString &", "key") << SingleArg("const QString &", "revision"), ArgsWithMandatoryDefaultValues_List() << SingleArgWithMandatoryDefaultValue("QString", "key", "QString()") << SingleArgWithMandatoryDefaultValue("QString", "revision", "QString()") << SingleArgWithMandatoryDefaultValue("QString", "data", "QString()"));
}
