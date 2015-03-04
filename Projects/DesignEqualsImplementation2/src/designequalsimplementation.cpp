#include "designequalsimplementation.h"

#include <QFile>
#include <QDataStream>

#include "designequalsimplementationproject.h"
#include "designequalsimplementationprojectserializer.h"

#ifdef DesignEqualsImplementation_TEST_MODE
#include <QDebug>
#include <QCoreApplication>
#include <QList>
#include <QDateTime>
#include "designequalsimplementationclass.h"
#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclassmethodargument.h"
#include "designequalsimplementationusecase.h"
#endif
#ifdef DesignEqualsImplementation_TEST_GUI_MODE
#include <QDebug>
#include <QCoreApplication>
#include "designequalsimplementationclass.h"
#endif

//TODOreq: random as fuck ish kinda OT, but actually not at all (despite my coming up with is idea while CODING THIS APP ITSELF): auto async/sync generation. you define a "slot" with a "bool" (generally 'success') return type, but the auto-generated async version uses a slot (the call) and signal (the 'return type') to satisfy it. however, the async call is backed/powered by a sync version, which uses a traditional public method and bool return variable. My point is this: when "designing", there is no difference between the two classes. However, upon code generation/etc, whether or not the objects are on the same thread (which is ultimately the deciding factor for whether or not async/sync is used anyways (is how Qt::AutoConnection behaves)) is what decides which of those two async/sync objects are used. I have not even begun to contemplate multiple uses of the same object by for example an object on the same thread AND an object on a different thread (when in doubt, async?).
//OT: :( I'm using a binary save format (QDataStream), but I still feel like these serialized designs belong in my /text/ repo. Could of course do XML/json/etc with ease ;-P
//TODOreq: thought about this while drifting around in between sleep/awake: a use case can have a [connected-to-NOTHING(auxilarySignalX)-during-previously-designed-use-case] signal as it's "entry point", but it isn't the same as the "first slot" entry point (and i'm glad, since that would require a bit of refactoring). it is more of a symbolic entry point (sort of like the ExitSignal) and not really a part of the "use case events". there isn't much more to write about, but there will likely be confusion when trying to implement that because of the two occurances of "entry points". one is "what triggers the use case" (actor or signal), and one is "the first use case event". OT'ish: deleting a signal in class diagram perspective used to trigger a use case needs to be handled properly, and i'm not sure how. perhaps the app keeps track of the use cases that are "invalid" (not triggered by either a signal or actor), and source can't be generated until it's fixed. or deleting triggering signal deletes that use case, BUT i'm not liking that idea much (best solution: ask right when/after signal is deleted what they want to do)
//TODOoptional: On a rainy day, templatify the source generation. It's definitely more powerful/flexible/etc, but I've learned from experience that whenever I do templates I get slowed down so much from the tediousness of it that I ragequit out of boredom
//TODOoptimization: when listening to our own signal and reacting to it, it would be a slight optimization (probably not worth this comment (unless you combine all the usages in all existence, then maybe)) to simply call the slot directly and then emit the signal (not actually connecting to it)
//honestly, graphical UML designers that don't generate functional use case code.... are fucking useless (haha pun but seriously). so simple, dia/umbrello suck
//TODOreq: a signal emit statement that uses a private method's return value for a signal argument would be extremely valuable
//***********TODOreq**********: a single signal needs to be able to connect to multiple slots (different class lifelines). right now I have to emit the same signal twice in order to connect to the second class lifeline's slot, but that has the unintended side effect of calling the first slot AGAIN! Actually err shit I mean err this has to do with "connect" statements and errr like uhh something about like err two emits in a row that are the same should be merged into ONE (but the connect statements still differentiated. I've never thought of this until just now.... fffff. NOTE: I'm referring to two class lifelines in the same use case listening to the same signal. Different use cases listening to signals emitted in another use case also needs to be considered, but is different-ish. For the same-use-case scenario, the signal arrow/line should perhaps "split into 2 (or N)".... idfk but this is pretty crucial...
//TODOoptional: the graphics item for "Generate Source Code" could be like a jack in the box. The icon is used again in the generate source code wizard dialog, but this time it changes depending on whether it was successful or not. If it's a failure, a hand with a middle finger pops out. If success, uhh I guess a regular jack in the box or perhaps MY SEXY FACE as a jack in the box. Bonus points if animated. The same middle finger icon should be used if ever the pre-generate-source-code stages fail as well (such as not having a class lifeline instanced, not having a slot named, etc etc). I thought about having my member pop out, but it's hard to determine whether that's negative/positive
DesignEqualsImplementation::DesignEqualsImplementation(QObject *parent)
    : QObject(parent)
{
    //TODOoptional: a static bool to protect these from double registering, if there are ever two instances of 'this' class ever instantiated at the same time
    qRegisterMetaType<UmlItemsTypedef>("UmlItemsTypedef");
    qRegisterMetaType<SignalEmissionOrSlotInvocationContextVariables>("SignalEmissionOrSlotInvocationContextVariables");
    qRegisterMetaType<DesignEqualsImplementationProject::ProjectGenerationMode>("DesignEqualsImplementationProject::ProjectGenerationMode");
    qRegisterMetaType<UseCaseEventListEntryType>("UseCaseEventListEntryType");
}
DesignEqualsImplementation::~DesignEqualsImplementation()
{
    qDeleteAll(m_CurrentlyOpenedDesignEqualsImplementationProjects);
}
void DesignEqualsImplementation::newProject()
{
#ifdef DesignEqualsImplementation_TEST_MODE
    //We have no GUI yet, so simulate GUI activity for development purposes (designEquals 1 started with GUI, designEquals 2 (this) is starting with backend)

    //TODOoptional: for most/all of these class instantiations (including the use case ones below), we can mandate associate with a parent to ease coding. Example, instead of "Class *foo = new Class", I could do Class *foo = project->newClass();

    DesignEqualsImplementationProject *testProject = new DesignEqualsImplementationProject(this);
    testProject->Name = "SimpleAsyncSlotInvokeAndSignalResponseProject";

    //Foo
    DesignEqualsImplementationClass *fooClass = new DesignEqualsImplementationClass(testProject);
    fooClass->ClassName = "Foo";
    DesignEqualsImplementationClassSlot *fooSlot = new DesignEqualsImplementationClassSlot(fooClass);
    fooSlot->Name = "fooSlot";
    DesignEqualsImplementationClassMethodArgument *fooSlotCuntArgument = new DesignEqualsImplementationClassMethodArgument(fooSlot);
    fooSlotCuntArgument->Name = "cunt";
    fooSlotCuntArgument->Type = "const QString &"; //TODOidfk: do I need the concept of a value? or is that only necessary at run-time?
    fooSlot->Arguments.append(fooSlotCuntArgument);
    fooClass->Slots.append(fooSlot); //TODOreq: can also be implicitly added via use case creation, but either method does the same thing (non-referenced classes are only taken out of the binary if the C++ compiler decides to do so)
    fooSlot->ParentClass = fooClass;

    //TODOmb: a templated, paramter-less, addClass could be used to simplify memory management... but we should then return a pointer to the class for customizing etc (or those could be parameters, or both)
    testProject->addClass(fooClass); //Bar gets added implicitly, not explicitly, to the project, by being added to the PrivateMembers of Foo itself. Foo hasA Bar. TODOreq: Bar should have Foo as parent, and be deleted in Foo's constructor, since Foo hasA Bar. It's confusing that I'm talking about both memory management and UML design "parent" (ParentClass)

    //Bar
    DesignEqualsImplementationClass *barClass = new DesignEqualsImplementationClass(testProject);
    barClass->ClassName = "Bar";
    DesignEqualsImplementationClassSlot *barSlot = new DesignEqualsImplementationClassSlot(barClass);
    barSlot->Name = "barSlot";
    DesignEqualsImplementationClassMethodArgument *barSlotCuntArgument = new DesignEqualsImplementationClassMethodArgument(barSlot);
    barSlotCuntArgument->Name = "cunt";
    barSlotCuntArgument->Type = "const QString &";
    barSlot->Arguments.append(barSlotCuntArgument);
    barClass->Slots.append(barSlot);
    barSlot->ParentClass = barClass;

    //TODOreq: gui responsible for asking user for variable NAME (default to "m_Bar", unless already exists (ensure not dupe, etc))
    QString userChosenVariableNameForFoosInstanceOfBar("m_Bar");
    fooClass->HasA_PrivateMemberClasses.append(qMakePair(userChosenVariableNameForFoosInstanceOfBar, barClass));

    //...bleh could continue defining the rest of the signals/slots just like above, BUT that really solves _NOTHING_ in terms of figuring the use case stuff out (the hardest part)...

#if 0
    //USE CASE ATTEMPT 0:
    DesignEqualsImplementationUseCase *testUseCase = new DesignEqualsImplementationUseCase(testProject, testProject);
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

    //USE CASE ATTEMPT 3:
    testUseCase->addEvent(activateConnectionBetween_FooCheckpointSignalAndBarSlot_Event); //fooSlot is implicitly the first event, so is not added (but wtf I'm pretty sure I need to specify fooSlot somewhere (maybe during construction of UseCase, as a sort of entry point?)). <-- that also kind of tells me in a way that "Foo" is... for design/generation purposes... owned by the UseCase (at the very least, a reference to it). What I am trying to say: the "starting point" of any use case needs to specifify a class and a slot on that class!
    testUseCase->addEvent(activateConnectionBetween_BarSignalAndHandleBarSignal_Event); //TODOoptimization: between barSignal and fooSignal
    //^^^^^^WAIT WHAT? that second event is activated INSIDE barSlot, so I am designing the slot bodies themselves... so does that mean i actually don't need someFooCheckpointSignal and that I had it right before? regardless, how do i let the use case know that fooSignal is my final signal? maybe, going back to what i was typing earlier, i should only define a sync interface and async should be generated for me (fooSignal becomes an auto-generated "return signal" in that case). Even the relationship between Foo and Bar can be defined both synchronously and asynchronously. It would be powerful if the code generation could handle both transparently (say, for example, at some point down the line, Bar is moved to another thread), but I'm not sure how to KISS in this instance...
    //TODOreq: ^^^If I do "Foo" generates "FooSync" and "FooAsync" behind the scenes, it's somehow worth mentioning that FooAsync uses FooSync, which uses BarSync (and never BarAsync. BarAsync could of course have been generated, but FooSync can't have used it. I suppose only that which is "used directly"... or perhaps better worded... at thread boundary... is used async)
    //I think the KISS design is to make everything async, because Qt::AutoConnection will make them sync if possible at run-time, and that also makes the code generated safer if modified (objects moved to other threads)
    //^^^^STILL, I have not (but should!?!?!?) defined the concept of async return types for a designed API. fooSignal is like a sibling in my eyes right now, but I'm considering making "pairing" it up with fooSlot design-wise. Still, not every slot invoke needs/warrants a signal/return (MAYBE: if that were the case, every app would be an infinite loop)
    //Is a use case entry point always a slot and an exit point always a signal? Perhaps that's the only pairing warranted...
    //Ok backing up again, after looking at UML and the original shit, fooSlot's body is being defined on the lifeline, so a barSlot invocation (transparently signals/slots) is fine and dandy, i don't need no fucking fooCheckpointSignal i'm pretty sure. And fuck I'm pretty tempted to say the same for handleBarSignal, _EXCEPT_ that instead of removing the existence of "handleBarSignal" like i just did for fooCheckpointSignal, I instead now think that "handleBarSignal" should be auto-generated (it SHOULD appear in design/gui and as a "slot" in the design) right when "barSignal" is... err... use case "connected" (connection activation event added to use case events list) to Foo
    //A reason I keep confusing myself: connection = signal/slot connection. connection = signal/slot connection activation in uml use case design view
    //TODOreq: signal/slot connection activation (indicated by use case timeline connection) should IMPLICITLY do the connection itself (there should NOT be any "connection view", although there could for fun/diagnostic (having it as utility (or primary method) would lead to lots of unused connections)), HOWEVER we might know know yet if Foo hasA Bar, Bar hasA Foo, or SomeClassX hasA Foo and a Bar... so where we put the connect code is kind of difficult to determine for my stupid fucking brain right now
    //^^Additionally, Foo might have a pointer to Bar and vice versa, and shit even in the SomeClassX example all 3 might have pointers to one another! Who "owns" them then and gets the connection code?. This problem is usually solved by the coder at code time, not by the designer at design time. But tbh it is pretty easy to solve (if I can wrap my fucking head around it)
    //For now I'm going to ignore the Bar hasA Foo where Foo is the use case entry point (just hurts my head. any real world cases?), but the other two are easy to determine where to put the connect code. Determining where to put the connect code is done at code generation time. If Foo hasA Bar, it goes in Foo's constructor. If not, Foo and Bar are neighbors and so the connect code goes right after wherever the Foo/Bar instantiations go
    //Are there any kind of use case events other than "signal/slot connection activations"? I can't think of any right now
    //What about fooSignal and it being the use case exit point?

    //USE CASE ATTEMPT 4:
    DesignEqualsImplementationClassSlot fooSlot;
    DesignEqualsImplementationUseCase fooBarAsyncTestUseCase(fooSlot /*use case entryPoint*/);
    DesignEqualsImplementationClass barClass;
    DesignEqualsImplementationClassSlot barSlot;
    fooBarAsyncTestUseCase.addEvent(barSlot); //TODOreq: signals going to no-one also addable via addEvent overload (for using later). This slot overload is a "signal/slot connection actication", and said signal/slot connection code is placed in Foo's constructor if Foo hasA Bar by the time the source code is generated, otherwise right next to Foo
    fooBarAsyncTestUseCase.addEvent(barSignal, handleBarSignal); //TODOreq: the user draws an arrow (connection activation) in the gui, names it "barSignal", and then from that we generate the handleBarSignal slot on whatever object the arrow points to
    fooBarAsyncTestUseCase.addEvent(fooSignal); //TODOreq: the user draws an arrow (connection activation) in the gui back to the actor, names it "fooSignal". since it is just going back to the actor, we don't need or generate a handleFooSignal slot anywhere. This overload is used for regular signals at any point in time during the use case, but also COINCIDENTALLY-BUT-NOT-MANDATORY at the use case exit point. Some use cases do not respond in any way, and that is fine.

    //attempt 4 was close, but i'm still not satisfied with fooSignal -> actor. that signals with no listeners [yet(design-wise)] can be emitted at any point in the use case is both crucial and confusing. i would think that there would always be a receiver, but then all of the sudden i'm now contemplating multiple use cases in one (one use case perhaps is triggered by a signal that previously had no listeners (at the time of desiging))
    //I also need/want a way to note that fooSignal _IS_ my use case exit point for this particular use case. it isn't just a signal occuring at the end of the use case! so that warrants some other method or overload or flag/etc, otherwise fooSignal won't be connected to Actor when I load the file next time (or my auto-UI generators (bleh, mindfuck) might not listen to fooSignal, foward it to user, and optionally quit the application [if CLI]
    //TODOreq: how the fuck should a signal-with-no-listeners-at-design-time even look visually? an arrow pointing up into outer-space? Dia's EventSource kind of thing would work actually... (and so my Actor should visually have an EventSink for the use case exit point i guess...)

    //USE CASE ATTEMPT 5:
    DesignEqualsImplementationUseCase useCase5;
    useCase5.addEvent(fooSlot); //because fooSlot might not exist when UseCase is instantiated. that it is added first make it obviously the entry-point
    useCase5.addEvent(barSlot); //TODOreq: see attempt 4's notes
    useCase5.addEvent(barSignal, handleBarSignal); //TODOreq: attempt 4 notes
    useCase5.addExitEvent(fooSignal);
#endif

    DesignEqualsImplementationUseCase *testUseCase = new DesignEqualsImplementationUseCase(testProject, testProject);
    testUseCase->Name = "SimpleAsyncSlotInvokeAndSignalResponseUseCase";

    testUseCase->addEvent(fooSlot);

    //diagnosticSignalX (no listeners)
    DesignEqualsImplementationClassSignal *diagnosticSignalX = new DesignEqualsImplementationClassSignal(fooClass);
    diagnosticSignalX->Name = "diagnosticSignalX";
    fooClass->Signals.append(diagnosticSignalX);

    testUseCase->addEvent(diagnosticSignalX);

    /*
    OnUserConnectedArrowsFromFooSlotToBarSlot:
    {
        //system notices that we already have context, which means that "assigning" (or they could be QStringLiterals hardcoded idgaf TODOreq) of the barSlot parameters given current variables in context is mandatory. this happens all in UI before addEvent(barSlot) below <-- ACTUALLY we have to ask the user which slot/signal/etc to use for the arrow anyways, so the variable name entering/selecting should go just below that. TODOreq: entering a variable TYPE (name too) that does not exist in the design should create that type in the class diagram via a "create in class diagram opt-out check box" [that is grayed out when class already exists or if the type is a C++ built-in]

        //populate stringlist of variables in current context, consisting of:
            0 = testUseCase->SlotWithCurrentContext->Arguments
            1 = testUseCase->SlotWithCurrentContext->ParentClass->allMyAvailableMemberGettersWhenInAnyOfMyOwnSlots_AsString(); TODOreq: de-serialization doesn't set up parent class again =o, but can/must
            2 = testUseCase->SlotWithCurrentContext->allMyAvailableLocalVariablesGivenCurrentPositionInOrderedListOfStatements_AsString()) -- TODOreq: only show compatible types (implicit conversion makes this a bitch, so fuck it for now. errors will only be seen at compile time lololol)

            TODOreq (disregard end of comment): need to decide if the fooSlot args are still available from within handleBarSignal, since they are the same unit of execution (since same thread)!!! design-wise, it makes sense. But it also makes sense that they not exist! I don't know! Perhaps I am being stupid by defining a unit of execution as anything not crossing thread boundary. Yes it is true, BUT .... but what? unit of executions don't make any guarantees about contexts... so wtf are you on about? Basically what led me here is that it feels weird to type currentUnitOfExecution->currentLifeLine,,,,, wait what? where does the slot come in (obviously I'm trying to get Foo's arg list...)... OH RIGHT the entire graphics scene is specific to that use case (which gives us access to slot with current context), i don't need to climb up the unit of execution chain, disregard this comment (although some of the beginning of it was relevant)

        //present stringlist of variables to user in modal widget
        //user selects fooSlot(__cunt__) to satify barSlot(__cunt__)
    }
    */
    QString nameOfVariableWithinCurrentContextsScopeToUseForBarSlotCunt = "cunt"; //same name = irrelevant coincidence. see pseudo-code directly above to understand how this string was magically populated
    QList<QString> barSlotArgs;
    barSlotArgs << nameOfVariableWithinCurrentContextsScopeToUseForBarSlotCunt; //etc
    //The name of Bar within Foo ("m_Bar") needs to be noted somewhere, but it can't be within Bar itself since other pointers to the same object owned by other classes might use a different name for it. Basically I need to refactor so that the "use case event" is a slot invocation on a PrivateMember of Foo, not just a "slot that exists in a class" (but I do still want to be able to add a "slot that exists in a class", because that's what the use case entry point will always be. confusing refactor)
    SignalEmissionOrSlotInvocationContextVariables barSlotInvocationContextVariables;
    barSlotInvocationContextVariables.VariableNameOfObjectInCurrentContextWhoseSlotIsAboutToBeInvoked = userChosenVariableNameForFoosInstanceOfBar; //TODOreq: UI selects (or JIT adds/creates) this when/right-after arrow is drawn from Foo
    barSlotInvocationContextVariables.OrderedListOfNamesOfVariablesWithinScopeWhenSignalEmissionOrSlotInvocationOccurrs_ToUseForSignalEmissionOrSlotInvocationArguments = barSlotArgs;
    testUseCase->addEvent(barSlot, barSlotInvocationContextVariables); //TODOreq: indication by user that fooSlot's "cunt" arg is used as barSlot "cunt" (that the args have the same name is coincidence). Somehow we need a list of variables accessible at the current context (members (including private) and current-slot-arguments are the main ones), and to specify one when addEvent(slot) is called. I'm not entirely sure said list needs to be available to "the programmer" after hitting a "." or typing a "->", but the use case is different: the user clicks-n-drags an arrow from Foo (fooSlot) to Bar (barSlot), and then AFTERWARDS they are presented a "slot argument population" wizard that has at it's disposal (even if just in "string" form (we are code GENERATOR, so strings are perfecto [even if that means the design is hacked])) all the variables right where the barSlot invocation was placed inside fooSlot
    //^It really boils down to a slot to be able to simply cough up it's own arguments, in addition to the members of the class it belongs to. There is also of course local context (1 or N lines above barSlot invocation), which will probably prove to be a bitch (*especially* if the lines above are hand-coded C++ (not even sure I'm allowing that so..)). As long as the local variables aren't hand-coded, keeping a list of local variables won't be that hard.
    //tl;dr:
    //0) current slot arguments
    //1) current slot's class's members
    //2) local variables of applicable list entries in Slot::OrderedListOfStatements that came before current slot invocation
    //...I'm going to start pseudo-coding above testUseCase->addEvent(barSlot); now, but know that all comments between here and there came first

    //barSignal and handleBarSignal
    DesignEqualsImplementationClassSignal *barSignal = new DesignEqualsImplementationClassSignal(barClass);
    barSignal->Name = "barSignal";
    DesignEqualsImplementationClassMethodArgument *barSignalSuccessArgument = new DesignEqualsImplementationClassMethodArgument(barSignal);
    barSignalSuccessArgument->Name = "success";
    barSignalSuccessArgument->Type = "bool";
    barSignal->Arguments.append(barSignalSuccessArgument);
    barClass->Signals.append(barSignal);
    DesignEqualsImplementationClassSlot *handleBarSignal = new DesignEqualsImplementationClassSlot(fooClass);
    handleBarSignal->Name = "handleBarSignal"; //TODOreq: generate name not already in use (this first one is most likely fine)
    DesignEqualsImplementationClassMethodArgument *handleBarSignalSuccessArgument = new DesignEqualsImplementationClassMethodArgument(handleBarSignal);
    handleBarSignalSuccessArgument->Name = "success";
    handleBarSignalSuccessArgument->Type = "bool";
    handleBarSignal->Arguments.append(handleBarSignalSuccessArgument);
    fooClass->Slots.append(handleBarSignal);
    handleBarSignal->ParentClass = fooClass;

    //TODOreq: Until there is C++ drop-down mode, we have to at least let the user type in a variable name for the signal emission. It's similar to the slot invocation in that it's asked right after the arrow is drawn. TODOreq: C++ core types (true, any number, etc) don't create classes in class diagram on the fly obviously
    SignalEmissionOrSlotInvocationContextVariables barSignalEmissionArguments;
    barSignalEmissionArguments.OrderedListOfNamesOfVariablesWithinScopeWhenSignalEmissionOrSlotInvocationOccurrs_ToUseForSignalEmissionOrSlotInvocationArguments.append("true");
    testUseCase->addEvent(barSignal, handleBarSignal, barSignalEmissionArguments);

    //fooSignal
    DesignEqualsImplementationClassSignal *fooSignal = new DesignEqualsImplementationClassSignal(fooClass);
    fooSignal->Name = "fooSignal";
    DesignEqualsImplementationClassMethodArgument *fooSignalSuccessArgument = new DesignEqualsImplementationClassMethodArgument(fooSignal);
    fooSignalSuccessArgument->Name = "success";
    fooSignalSuccessArgument->Type = "bool";
    fooSignal->Arguments.append(fooSignalSuccessArgument);
    fooClass->Signals.append(fooSignal);

    SignalEmissionOrSlotInvocationContextVariables exitSignalEmissionArguments;
    exitSignalEmissionArguments.OrderedListOfNamesOfVariablesWithinScopeWhenSignalEmissionOrSlotInvocationOccurrs_ToUseForSignalEmissionOrSlotInvocationArguments.append("success");
    testUseCase->setExitSignal(fooSignal, exitSignalEmissionArguments);

    //To test ExitSignal auto-moving for UseCaseSignalEventType, uncomment this line AND comment out the identical line up above
    //testUseCase->addEvent(diagnosticSignalX);

    //To test ExitSignal auto-moving for UseCaseSlotEventType and UseCaseSignalSlotEventType simultaneously, uncomment these next two lines AND comment out their identical lines up above
    //testUseCase->addEvent(barSlot, barSlotInvocationContextVariables);
    //testUseCase->addEvent(barSignal, handleBarSignal, barSignalEmissionArguments);

    testProject->addUseCase(testUseCase);
    //TODOreq: either make single use case default use case in project during generation, or require specifying it here. In the final GUI we'd ask the user ofc

    //generate source
    connect(testProject, SIGNAL(sourceCodeGenerated(bool)), this, SLOT(handlesourceCodeGenerated(bool)));
    connect(testProject, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
    connect(this, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    testProject->generateSourceCode(DesignEqualsImplementationProject::Library, "/run/shm/designEqualsImplementation-GeneratedAt_" + QString::number(QDateTime::currentMSecsSinceEpoch()));

    emit projectOpened(testProject);
#endif // DesignEqualsImplementation_TEST_MODE
#ifdef DesignEqualsImplementation_TEST_GUI_MODE
    DesignEqualsImplementationProject *testProject = new DesignEqualsImplementationProject(this);
    testProject->Name = "TestProject";

    //Foo
    DesignEqualsImplementationClass *fooClass = testProject->createNewClass("Foo");
    DesignEqualsImplementationClassSlot *fooSlot = fooClass->createwNewSlot("fooSlot");
    fooClass->createwNewSlot("blahFooSlot");
    DesignEqualsImplementationClassMethodArgument *fooSlotCuntArgument = fooSlot->createNewArgument("const QString &", "cunt");

    DesignEqualsImplementationClassSignal *diagnosticSignalX = fooClass->createNewSignal("diagnosticSignalX");
    fooClass->createNewSignal("anotherSignal");

    //fooSignal
    DesignEqualsImplementationClassSignal *fooSignal = fooClass->createNewSignal("fooSignal");
    DesignEqualsImplementationClassMethodArgument *fooSignalSuccessArgument = fooSignal->createNewArgument("bool", "success");

    //handleBarSignal -- would be auto-generated ideally, but for now I just want to be able to test with it already there
    DesignEqualsImplementationClassSlot *handleBarSignal = fooClass->createwNewSlot("handleBarSignal");
    DesignEqualsImplementationClassMethodArgument *handleBarSignalSuccessArgument = handleBarSignal->createNewArgument("bool", "success");

    //Bar
    DesignEqualsImplementationClass *barClass = testProject->createNewClass("Bar");
    DesignEqualsImplementationClassSlot *barSlot = barClass->createwNewSlot("barSlot");
    barClass->createwNewSlot("blahBarSlot");
    DesignEqualsImplementationClassMethodArgument *barSlotCuntArgument = barSlot->createNewArgument("const QString &", "cunt");

    DesignEqualsImplementationClassSlot *barSlot1 = barClass->createwNewSlot("barSlot1");
    DesignEqualsImplementationClassSlot *barSlot2 = barClass->createwNewSlot("barSlot2");

#if 0
    //bool Bar::m_Success -- HACK'ish: need something to satisfy barSignal's success arg
    HasA_Private_PODorNonDesignedCpp_Members_ListEntryType *successPodMember = barClass->createNewHasAPrivate_PODorNonDesignedCpp_Member("bool", "m_Success");
#endif

    //barSignal
    DesignEqualsImplementationClassSignal *barSignal = barClass->createNewSignal("barSignal");
    DesignEqualsImplementationClassMethodArgument *barSignalSuccessArgument = barSignal->createNewArgument("bool", "success");

    barClass->createNewSignal("barSignalBlah0");
    barClass->createNewSignal("barSignalBlah1");

    QString userChosenVariableNameForFoosInstanceOfBar("m_Bar"); //TODOreq: i imagine these are set elsewhere (maybe i response to a user event. as in, they name it (imagine that))
    fooClass->createHasA_Private_Classes_Member(barClass, userChosenVariableNameForFoosInstanceOfBar);
    //fooClass->HasA_Private_Classes_Members.append(new DesignEqualsImplementationClassInstance(barClass, fooClass/*fooClassInstance*/, userChosenVariableNameForFoosInstanceOfBar));

    //Zed
    DesignEqualsImplementationClass *zedClass = testProject->createNewClass("Zed");

    zedClass->createwNewSlot("blahZedSlot");

    //zedSignal
    zedClass->createNewSignal("zedSignal");

    DesignEqualsImplementationClassSlot *zedSlot = zedClass->createwNewSlot("zedSlot");
    DesignEqualsImplementationClassMethodArgument *zedSlotCuntArgument = zedSlot->createNewArgument("const QString &", "cunt");

    fooClass->createHasA_Private_Classes_Member(zedClass, "m_FoosCopyOfZed");
    barClass->createHasA_Private_Classes_Member(zedClass, "m_BarsCopyOfZed");
    //barClass->HasA_Private_Classes_Members.append(new DesignEqualsImplementationClassInstance(zedClass, barClass, userChosenVariableNameForBarsInstanceOfZed));

    m_CurrentlyOpenedDesignEqualsImplementationProjects.append(testProject); //could do this->createProject, but then wouldn't be able to populate it before emitting it as opened lawl (or i guess i could)
    connect(testProject, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(testProject, SIGNAL(sourceCodeGenerated(bool)), this, SLOT(handlesourceCodeGenerated(bool)));
    emit projectOpened(testProject);
#endif // DesignEqualsImplementation_TEST_GUI_MODE
    DesignEqualsImplementationProject *newProject = new DesignEqualsImplementationProject(this);
    newProject->Name = "NewProject1"; //TODOreq: auto-increment. TODOoptional: human readable variant with spaces (for window title)? mb project name generated FROM human readable name by taking out spaces (two line edits, the second one (project name) auto-generates as you type human readable name)

    connect(newProject, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
    m_CurrentlyOpenedDesignEqualsImplementationProjects.append(newProject);
    emit projectOpened(newProject);
}
void DesignEqualsImplementation::saveProject(DesignEqualsImplementationProject *projectToSave, const QString &projectFilePath)
{
    //vrooooom
    QFile projectFile(projectFilePath);
    if(!projectFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) //TODOreq: gui confirms overwrite. for now i am doing monolithic/single-state saves, but i'm considering migrating to mutations/replayable(undo-framework)
    {
        emit e("could not open file '" + projectFilePath + "' for writing");
        return;
    }
    //QDataStream projectDataStream(&projectFile);
    //oops: projectDataStream << this; //surprisingly anti-climactic (but still awesome in it's own way)
    //projectDataStream << *projectToSave;
    DesignEqualsImplementationProjectSerializer projectWriter(this);
    projectWriter.serializeProjectToIoDevice(projectToSave, &projectFile);

    if(!projectFile.flush())
    {
        emit e("could not flush to file '" + projectFilePath + "'");
        return;
    }
}
void DesignEqualsImplementation::openExistingProject(const QString &existingProjectFilePath)
{
    QFile projectFile(existingProjectFilePath);
    if(!projectFile.open(QIODevice::ReadOnly))
    {
        emit e("could not open file '" + existingProjectFilePath + "' for reading");
        return;
    }
    //QDataStream projectDataStream(&projectFile);
    DesignEqualsImplementationProject *existingProject = new DesignEqualsImplementationProject(this);
    connect(existingProject, SIGNAL(e(QString)), this, SIGNAL(e(QString)));

    //projectDataStream >> *existingProject;
    DesignEqualsImplementationProjectSerializer projectReader(this);
    projectReader.deserializeProjectFromIoDevice(&projectFile, existingProject);

    m_CurrentlyOpenedDesignEqualsImplementationProjects.append(existingProject);
    emit projectOpened(existingProject);
}
#ifdef DesignEqualsImplementation_TEST_GUI_MODE
void DesignEqualsImplementation::handlesourceCodeGenerated(bool success)
{
    qDebug() << "Source code generated successfully: " << success;
    //QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
void DesignEqualsImplementation::handleE(const QString &msg)
{
    qDebug() << msg;
}
#endif
