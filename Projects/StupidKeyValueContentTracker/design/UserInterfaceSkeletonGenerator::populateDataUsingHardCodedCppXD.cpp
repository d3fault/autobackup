void UserInterfaceSkeletonGenerator::populateDataUsingHardCodedCppXD(UserInterfaceSkeletonGeneratorData &data)
{
    data.BusinessLogiClassName = "StupidKeyValueContentTracker";


    data.createAndAddSlot("void", "add", ArgsList() << SingleArg{"const QString &","key"} << SingleArg{"const QString &","data"} );
    data.createAndAddSlot("void", "commit", ArgsList() << SingleArg{"const QString &","commitMessage"});
    data.createAndAddSlot("void", "readKey", ArgsList() << SingleArg{"const QString &","key"} << SingleArg{"const QString &","revision"} );

    data.createAndAddSignal("e", ArgsList() << SingleArg{"QString","msg"});
    data.createAndAddSignal("o", ArgsList() << SingleArg{"QString","msg"});
    data.createAndAddSignal("addFinished", ArgsList() << SingleArg{"bool","success"});
    data.createAndAddSignal("commitFinished", ArgsList() << SingleArg{"bool","success"});
    data.createAndAddSignal("readKeyFinished", ArgsList() << SingleArg{"bool","success"} << SingleArg{"QString","key"} << SingleArg{"QString","revision"} << SingleArg{"QString","data"});
}
