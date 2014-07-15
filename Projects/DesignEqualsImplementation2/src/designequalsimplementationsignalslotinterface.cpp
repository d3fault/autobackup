#include "designequalsimplementationsignalslotinterface.h"

#if 0

//The interface jacks tha name from the implementation as it is being created, but leaves it behind should it be deleted from a use case
//Was tempted to use just the signal/slot pointers of this app instead of qstrings/copies, but i dont want them changing out from under my use cases (do i?)
//Fuck the interfaces, I just want use cases to use slots (just decided to rename units of executions to slot)
//TODOreq: do SEPARATE uml editor for synchronous/not-thread-safe mode (time critical code): flow chart. each call is synchronous, so it IS ordered in it's entirety, unlike the asynchronous/thread-safe signals/slots mode by default. i should just focus on tthe thread safe one only. i'm wasting concentration and trying to abstract too hard for both. Which also means fuck this interface stuff for now. Basically I _DO_ want a slot that's unnamed to be able to have statements added to it. Hell, it doesn't even need to have a parent class name yet. Hell, it never does (you are prompted to fill in a list when clicking generate source code (and links to those list entries given), but you can just say "leave unnamed (auto-generate temporary placeholders just for this source generate)"
DesignEqualsImplementationSignalSlotInterface::DesignEqualsImplementationSignalSlotInterface(QObject *parent)
    : QObject(parent)
{ }
#endif
