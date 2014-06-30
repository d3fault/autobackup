#include "designequalsimplementation.h"

#define DesignEqualsImplementation_TEST_MODE 1

#ifdef DesignEqualsImplementation_TEST_MODE
#include <QList>
#include "designequalsimplementationclass.h"
#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclassmethodargument.h"
#include "designequalsimplementationusecase.h"
#include "designequalsimplementationusecaseevent.h"
#endif

//TODOreq: random as fuck ish kinda OT, but actually not at all (despite my coming up with is idea while CODING THIS APP ITSELF): auto async/sync generation. you define a "slot" with a "bool" (generally 'success') return type, but the auto-generated async version uses a slot (the call) and signal (the 'return type') to satisfy it. however, the async call is backed/powered by a sync version, which uses a traditional public method and bool return variable. My point is this: when "designing", there is no difference between the two classes. However, upon code generation/etc, whether or not the objects are on the same thread (which is ultimately the deciding factor for whether or not async/sync is used anyways (is how Qt::AutoConnection behaves)) is what decides which of those two async/sync objects are used. I have not even begun to contemplate multiple uses of the same object by for example an object on the same thread AND an object on a different thread (when in doubt, async?).
DesignEqualsImplementation::DesignEqualsImplementation(QObject *parent)
    : QObject(parent)
{ }
DesignEqualsImplementation::~DesignEqualsImplementation()
{
    qDeleteAll(m_CurrentlyOpenedDesignEqualsImplementationProjects);
}
void DesignEqualsImplementation::newEmptyProject()
{
#ifdef DesignEqualsImplementation_TEST_MODE
    //We have no GUI yet, so simulate GUI activity for development purposes (designEquals 1 started with GUI, designEquals 2 (this) is starting with backend)

    DesignEqualsImplementationProject *testProject = new DesignEqualsImplementationProject(this);
    testProject->Name = "SimpleAsyncSlotInvokeAndSignalResponse";
    DesignEqualsImplementationClass *testClass = new DesignEqualsImplementationClass(testProject);
    testClass->Name = "Foo";
    DesignEqualsImplementationClassSlot *testSlot = new DesignEqualsImplementationClassSlot(testClass);
    testSlot->Name = "fooSlot";
    DesignEqualsImplementationClassMethodArgument *testArgument = new DesignEqualsImplementationClassMethodArgument(testSlot);
    testArgument->Name = "cunt";
    testArgument->Type = "const QString &"; //TODOidfk: do I need the concept of a value? or is that only necessary at run-time?
    testSlot->Arguments.append(testArgument);
    testClass->Slots.append(testSlot); //TODOreq: can also be implicitly added via use case creation, but either method does the same thing (non-referenced classes are only taken out of the binary if the C++ compiler decides to do so)

    //...bleh could continue defining the rest of the signals/slots just like above, BUT that really solves _NOTHING_ in terms of figuring the use case stuff out (the hardest part)...


    //USE CASE ATTEMPT 0:
    DesignEqualsImplementationUseCase *testUseCase = new DesignEqualsImplementationUseCase(testProject);
    DesignEqualsImplementationUseCaseEvent *testUseCaseEvent = new DesignEqualsImplementationUseCaseEvent(testUseCase);
    testUseCaseEvent->Sender = "Actor"; //hhhhhhmmmmmmmmmmmmmmmmmmmmm
    testUseCaseEvent->Receiver = testSlot;
    testUseCase->OrderedUseCaseEvents.append(testUseCaseEvent); //TODOreq: receiver/slot's owning class is also implicitly added to class diagram (if it wasn't already) at some point during this call
    testProject->UseCases.append(testUseCase);

    //USE CASE ATTEMPT 1:
    testUseCase->setRootSlotInvocation(testSlot);

    //USE CASE ATTEMPT 2:
    testUseCase->addEvent(testSlot);
    DesignEqualsImplementationClassSlot *barSlot = new DesignEqualsImplementationClassSlot(testUseCase);
    testUseCase->addEvent(barSlot);
    //TODOreq: by adding both slot events in a row like this (allows me to not specify sender, which allows me to add the testSlot (sender = actor)), I'm not able to know whether Foo hasA Bar or if Foo and Bar are siblings (both async called back to back). I need to define connections, BUT ALSO when the connection is activated. I seem to have it down fine and dandy aside from the Actor not being a valid Sender problem...

    //rambling:
    //"Foo::fooSlot(string cunt)" is the core of the design. A CLI app might invoke it (populating cunt with an argv), wait for signal response, output result, and end the app. A GUI app might provide a line edit and push button for populating cunt with said line edit, and display responses in a qplaintextedit, but allow multiple of such invocations during a single run. My point is this: in the GUI version, Foo::fooSlot is connected to a QPushButton's "clicked", but in the CLI version we just invokeMethod the fooSlot. This application needs to not give a shit about the UI, so [...?...]. It's barely worth noting that I should be able to generate both CLI and GUI versions.
    //Connection lifetims are something that need to be defined, unless invokeMethod is littered everywhere. Still, connections are still extremely helpful in the tons of places when we aren't the "emitter" of said "signal"... so connection lifetime needs to be defined regardless of whether my generated classes use them
    //I kind of touched on this already in the fooSlot C++ impl (what I am looking at to try to get this working): I can lazy instantiate (and optionally connect-to (or just invoke)) Bar when needed, or not. "Lifeline" is what it's called in Dia (bleh Umbrello sucks, keeps crashing), so it's pretty much what I need to define in this app too...
    //Class diagram generates skeletons use cases (lifelines, or "connections" even, if you will) depending on who hasA what... the use cases themselves merely activate said connections (or send events from lifelines to lifelines). TODOoptional: by default when Class0 hasA Class1, the instantiation and connections will be done in Class0 constructor. We could introduce a flag so that Class1 is lazy loaded. I still dunno about how/when to handle destruction/disconnection (and there is probably a corresponding flag that makes the most sense in that stage too), but eh one problem at a time
    //OT'ish (staring too much): I can't figure out anymore why I'd ever want a class to haveA different class IF THE TWO CLASSES WILL ONLY COMMUNICATE VIA SIGNALS/SLOTS/PROPERTIES. It really makes very little difference if they are siblings simply connected or one hasA the other (also connected). Hell, even the "child" in such relationship (bar) could own a parent (foo) and bleh it wouldn't mess much up except for the initial fooSlot invocation (which I still have yet to figure out). Perhaps it is just sanity and organization why one class would have another (OO really does ultimately become procedural in the end anyways), and my intentionally trying to abstract upon it is making my brain collapse upon itself. Still wondering if I need to add a "QList<DesignEqualsImplementationClass*> m_HasAClasses;" to DesignEqualsImplementationClass
    //The first paragraph of these ramblings is the most important. The actor can be a signal or not, it is irrelevant to the design itself (except upon generation ofc (TODOreq: ui-less mode where it stays irrelevant too heh (library mode?))). I feel like my invocation of Bar via auto-generated-signal (or auto generated invokeMethod) is incorrect, and that I should have instead defined a "someCheckpointSignal" on Foo that barSlot was listening for. Connecting a slot to a slot is both stupid and smart (smart because ultimately such things happen all the fucking time, but stupid because now I'm just confusing myself aka over optimizing. that "signal" would be invisible/auto-generated in the slot->slot case, so I am accidentally doing an optimization FIRST. time to commit and change that)

#else
    m_CurrentlyOpenedDesignEqualsImplementationProjects.append(new DesignEqualsImplementationProject(this));
#endif
}
void DesignEqualsImplementation::loadProjectFromFilePath(const QString &existingProjectFilePath)
{
    m_CurrentlyOpenedDesignEqualsImplementationProjects.append(new DesignEqualsImplementationProject(existingProjectFilePath, this));
}
