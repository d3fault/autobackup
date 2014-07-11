#include "designequalsimplementationusecase.h"

#include <QDataStream>

#include "designequalsimplementationclass.h"
#include "designequalsimplementationactor.h"
#include "designequalsimplementationclasslifeline.h"
#include "designequalsimplementationclasslifelineunitofexecution.h"
#include "designequalsimplementationslotinvocationstatement.h"
#include "designequalsimplementationsignalemissionstatement.h"

typedef QPair<DesignEqualsImplementationUseCase::UseCaseEventTypeEnum, QObject*> EventAndTypeTypedef;

#define DesignEqualsImplementationUseCase_QDS(direction, qds, useCase) \
qds direction useCase.Name; \
qds direction numOrderedUseCaseEvents;
//qds direction hasExitSignal;

//TODOreq: by far the most difficult class to design/[de-]serialize/generate-from. the rest are just busywork by comparison
//TODOoptimization: class diagram serialized first (although use cases must be factored into that!), serves as a simple library or whatever so that use cases only later have to refer to a signal by id/etc rather than redefine/reserialize the signal/slot/etc every time it is referenced
//TODOreq: since it doesn't have much else use, use this->Name in the comments of the generated source code. the use case entry point slot should say the use case name for readability i suppose. also, if verbose logging is on, then each time a use case entry point is crossed we could output the name...
//TODOreq: right now my use case events only function in "append" mode. Obviously I need to be able to insert at arbitrary points. If I am unable to pro that solution, I could always "replay" the appending of use case events, this time now appending the to-be-inserted use case event at the proper time... and then discarding the old order list of use case events
//The concept of unit of execution does make sense in UML when dealing with the C++ non-designed types. UML use cases are still useful for dealing with those, and they make no thread safety guarantees. It almost sounds like I'm talking about two apps, or two modes of the same app at least. They are all direct function calls so they do make a unit of execution guarantee. See? two MODES of "UML Use Case Designing", wtf
//TODOoptional: generic async waiters, "wait for this signal from this object and that signal from that object (N of such signals/objects), THEN do this slot". A QList of bools or something. The main thing is that the async operations are started right when they are added, but that might mandate a (transparent/auto-generated?) "doneAddingAsyncShits" call just like in ObjectOnThreadGroup
DesignEqualsImplementationUseCase::DesignEqualsImplementationUseCase(QObject *parent)
    : QObject(parent)
{
    privateConstructor(0); //TODOreq: this constructor was made just for deserializing. make sure project isn't used without first being checked (however, this bug would only ever occur when opening a saved file (and I've already made a note to manually set the project))
}
DesignEqualsImplementationUseCase::DesignEqualsImplementationUseCase(DesignEqualsImplementationProject *project, QObject *parent)
    : QObject(parent)
{
    privateConstructor(project);
}
DesignEqualsImplementationProject *DesignEqualsImplementationUseCase::designEqualsImplementationProject() const
{
    return m_DesignEqualsImplementationProject;
}
bool DesignEqualsImplementationUseCase::generateSourceCode(const QString &destinationDirectoryPath)
{
#if 0
    bool firstUseCaseEvent = true;
    Q_FOREACH(EventAndTypeTypedef *orderedUseCaseEvent, OrderedUseCaseEvents)
    {
        switch(orderedUseCaseEvent->first)
        {
        case UseCaseSlotEventType: //changes context of next use case event
        {
            DesignEqualsImplementationClassSlot *slotUseCaseEvent = static_cast<DesignEqualsImplementationClassSlot*>(orderedUseCaseEvent->second);
            if(firstUseCaseEvent)
            {
                //we have no context, but want the first slot to be our context! TODOreq: there is still ui/cli context to consider, in which case we would have a context already at this point. if this project is generating in lib mode, we don't
                //generate slot context/empty-impl from current use case event, then set that slot/context/empty-impl as current context
                firstUseCaseEvent = false;
            }
            else
            {
                //generate slot context/empty-impl from current use case event, then in current context do nameless-signal or invokeMethod to the slot/context/empty-impl, then set that slot/context/empty-impl as current context
            }
        }
            break;
        case UseCaseSignalEventType: //does not change context of next use case event
        {
            DesignEqualsImplementationClassSignal *signalUseCaseEvent = static_cast<DesignEqualsImplementationClassSignal*>(orderedUseCaseEvent->second);
            //in current context, emit the signal
        }
            break;
        case UseCaseSignalSlotEventType: //changes context of next use case event. the only difference from UseCaseSignalSlotEventType and UseCaseSlotEventType is that the signal is a named part of the design for UseCaseSignalSlotEventType, whereas UseCaseSlotEventType either uses an autogenerated nameless signal or invokeMethod (doesn't matter which i choose)
        {
            SignalSlotCombinedEventHolder *signalSlotCombinedUseCaseEvent = static_cast<SignalSlotCombinedEventHolder*>(orderedUseCaseEvent->second);

        }
            break;
        case UseCaseExitSignalEventType: //handled differently (although i could...)
            break;
        }
    }
    if(ExitSignal)
    {
        //TODOreq
    }
#endif
    return true;
}
DesignEqualsImplementationUseCase::~DesignEqualsImplementationUseCase()
{
    //TODOreq: probably don't need to delete here AND in class diagram perspective, but probably doesn't hurt also
    Q_FOREACH(EventAndTypeTypedef eventAndType, SemiOrderedUseCaseEvents)
    {
        //delete eventAndType.second;
        if(eventAndType.first == UseCaseSignalSlotEventType) //all other kinds are deleted via class diagram perspective
        {
            delete eventAndType.second;
        }
    }
    /*if(ExitSignal)
        delete ExitSignal;*/
}
void DesignEqualsImplementationUseCase::privateConstructor(DesignEqualsImplementationProject *project)
{
    //SlotWithCurrentContext = 0;
    //ExitSignal = 0;
    m_DesignEqualsImplementationProject = project;
}
void DesignEqualsImplementationUseCase::insertEventPrivate(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventType, int indexToInsertEventAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassLifeLineUnitOfExecution *destinationUnitOfExecution_OrZeroIfDestIsActor, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot)
{
    //bool firstUseCaseEventAdded = OrderedUseCaseEvents.isEmpty();

    //Add it (mainly used for saving/opening ([de-]serialization))
    SemiOrderedUseCaseEvents.append(qMakePair(useCaseEventType, event));

    //Code generation and context setup
    switch(useCaseEventType)
    {
    case UseCaseSlotEventType: //changes context of next use case event
    {
        DesignEqualsImplementationClassSlot *slotUseCaseEvent = static_cast<DesignEqualsImplementationClassSlot*>(event);
        //if(firstUseCaseEventAdded) //first event add = use case entry point (TODOreq: ui sanitization so that a non-slot can never be made first (TODOoptional: file load sanitization so that we don't crash if it does happen (fuckit)))
        //{
            //we have no context, but want the first slot to be our context! TODOreq: there is still ui/cli context to consider, in which case we would have a context already at this point. if this project is generating in lib mode, we don't
            //generate slot context/empty-impl from current use case event, then set that slot/context/empty-impl as current context
#if 0
            if(SlotWithCurrentContext)
            {
                //TODOreq: ui already gave us context, so do nameless-signal/invokeMethod on ui to get to this slot, then just change the context as if we were in lib gen mode (fuck. we aren't even generating right now so..... wtf?)
            }
            else
            {
                SlotWithCurrentContext = slotUseCaseEvent;
            }
#endif
        //}
        //else
        //{
            //generate slot context/empty-impl from current use case event, then in current context do nameless-signal or invokeMethod to the slot/context/empty-impl, then set that slot/context/empty-impl as current context

            //TODOreq: source unit of execution gets the new statement, dest gets entire new unit of execution if in thread-safe mode, but just it's own set of statements (or just the referring to a set of statements) if not. Either way, the dest gets the "receive" message slot/method (depending which mode). What to name the "request" (if thread-safe-mode) and the "requestHandler" in either mode is asked right at/when/after arrow is drawn. If the slot-or-method has already been implemented in another use case, it should appear empty aside from all the signals it emits in it's use case and a finished/exit-signal in case you only want to know when it's done. Double clicking on that already implemented slot-or-private-method (both VISUALLY represented by something along the same lines as "unit of execution" rectangles (but hopefully differentiable with extreme prejudice) should to start off with open the use case it was defined in in a new tab, but in the future it would be awesome if it could inline expand into your current use case. Keeping with that train of thought, An application is just a 3d amusement park of electricity going to various places of heirarchial mountains. A specific use case is the zooming in of just one of those "electricity christmas strands of light" and putting the concept of an actor around (folding around you) both your start and exit points. What I'm trying to get at and what I'm not even sure is possible, is that somehow using 3d representation, use cases can link to each other in a "one definition" environment (the program could really be visualized like that)
            //The source has to determine where to insert the statement, but the dest always puts the statement as the first entry. When in C++/not-thread-safe mode, the order is retained so in that case a method call (dest!) might not be the first statement after all
            //TODOreq: etc for all switch types
            if(sourceOrderedListOfStatements_OrZeroIfSourceIsActor)
            {
                sourceOrderedListOfStatements_OrZeroIfSourceIsActor->OrderedListOfStatements.insert(indexToInsertEventAt, new DesignEqualsImplementationSlotInvocationStatement(slotUseCaseEvent, signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot));
            }

            //TODOreq: the dest unit of execution gets "named" (has entry point now, but it already existed prior to drawing the arrow) and is more or less a slot now (which can become (and possibly already is) a use case entry point). In C++ mode it is SEEN differently, but I think functions mostly the same
            if(destinationUnitOfExecution_OrZeroIfDestIsActor && !destinationUnitOfExecution_OrZeroIfDestIsActor->MethodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution) //hack to detect whether or not it's set/named
            {
                destinationUnitOfExecution_OrZeroIfDestIsActor->MethodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution = slotUseCaseEvent; //by setting the pointer, we are making the use case and slot basically one
            }

            //TODOreq: additionally, the connection as noted in use case should probably have a backend model

#if 0
            //OLD
            SlotWithCurrentContext->OrderedListOfStatements.append(new DesignEqualsImplementationSlotInvocationStatement(slotUseCaseEvent, signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot));
            SlotWithCurrentContext = slotUseCaseEvent;
#endif
        //}
    }
        break;
    case UseCaseSignalEventType: //does not change context of next use case event
    {
        DesignEqualsImplementationClassSignal *signalUseCaseEvent = static_cast<DesignEqualsImplementationClassSignal*>(event);
        //in current context, emit the signal

        if(sourceOrderedListOfStatements_OrZeroIfSourceIsActor)
        {
            sourceOrderedListOfStatements_OrZeroIfSourceIsActor->OrderedListOfStatements.insert(indexToInsertEventAt, new DesignEqualsImplementationSignalEmissionStatement(signalUseCaseEvent, signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot));
        }
        //OLD
        //SlotWithCurrentContext->OrderedListOfStatements.append(new DesignEqualsImplementationSignalEmissionStatement(signalUseCaseEvent, signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot));
    }
        break;
    case UseCaseSignalSlotEventType: //changes context of next use case event. the only difference from UseCaseSignalSlotEventType and UseCaseSlotEventType is that the signal is a named part of the design for UseCaseSignalSlotEventType, whereas UseCaseSlotEventType either uses invokeMethod (could have been an autogenerated nameless signal, but doesn't matter which)
    {
        //TODOreq: right now is when we need to make a note that the signal would be connected in the slot, and optimally resolving where that connection will be made is going to be a bitch :-P

        SignalSlotCombinedEventHolder *signalSlotCombinedUseCaseEvent = static_cast<SignalSlotCombinedEventHolder*>(event);

        if(sourceOrderedListOfStatements_OrZeroIfSourceIsActor)
        {
            sourceOrderedListOfStatements_OrZeroIfSourceIsActor->OrderedListOfStatements.insert(indexToInsertEventAt, new DesignEqualsImplementationSignalEmissionStatement(signalSlotCombinedUseCaseEvent->m_DesignEqualsImplementationClassSignal, signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot));
        }

        //TODOreq: dest

        //OLD
        //SlotWithCurrentContext->OrderedListOfStatements.append(new DesignEqualsImplementationSignalEmissionStatement(signalSlotCombinedUseCaseEvent->m_DesignEqualsImplementationClassSignal, signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot));
        //SlotWithCurrentContext = signalSlotCombinedUseCaseEvent->m_DesignEqualsImplementationClassSlot;
        //TODOreq: QObject::connect line generation. Multiple use cases can use the same connections, so this use case just puts a "dependency" on a connection, that any other use case can also put +1 on. Removing the connection activation in a use case puts subtracts 1 from that dependency. Any connection with > 0 dependencies gets put in the constructor (had: "and serialized for next time", but I'm not sure that's a good elimination criteria). The connect statements should go in whichever of the two (signal or source) is closer to the topmost parent
        //Hmm, does it really make sense for different use cases to use the same connection as another use case? I'm not so sure about that claim... because then the use cases would merge (or something)...
#if 0 //fuck this shit, waiting for proper which uses the class lifeline to get VariableName
        /*
        in the simplest (hacked in) case:
        if(slot.ClassParent is not in signal.ClassParent's HasA_Private_Classes_Members, and slot.ClassParent hasA exactly one of Signal.ClassParent in it's HasA_Private_Classes_Members)
        {
            the "connect" statements go in slot.ClassParent' constructor
        }
        */
        //Above implemented:
        if(!signalSlotCombinedUseCaseEvent->m_DesignEqualsImplementationClassSignal->ParentClass->HasA_Private_Classes_Members.contains(signalSlotCombinedUseCaseEvent->m_DesignEqualsImplementationClassSlot->ParentClass))
        {
            if(signalSlotCombinedUseCaseEvent->m_DesignEqualsImplementationClassSlot->ParentClass->HasA_Private_Classes_Members.count(signalSlotCombinedUseCaseEvent->m_DesignEqualsImplementationClassSignal->ParentClass) == 1)
            {
                //the "connect" statements go in slot.ClassParent' constructor
                //TODOreq: proper this shit
                HasA_Private_Classes_Members_ListEntryType *signalParentClassAsMemberType = signalSlotCombinedUseCaseEvent->m_DesignEqualsImplementationClassSlot->ParentClass->HasA_Private_Classes_Members;
                //signalSlotCombinedUseCaseEvent->m_DesignEqualsImplementationClassSlot->ParentClass->addInitializeStatement(new ConnectStatement)
            }
        }
#endif
    }
        break;
    }
#if 0
    /*
        BEFORE/AFTER below refers to the addEventPrivateWithoutUpdatingExitSignal call

        UseCaseSlotEventType:
            context BEFORE == m_ObjectCurrentyWithExitSignalInItsOrderedListOfStatements, takeAt + append AFTER, but to BEFORE's context
            context AFTER == m_ObjectCurrentyWithExitSignalInItsOrderedListOfStatements, takeAt + append [after implied]

        UseCaseSignalEventType:
            context BEFORE-or-AFTER (no context change) == m_ObjectCurrentyWithExitSignalInItsOrderedListOfStatements, takeAt + append [after implied, so check after]

        UseCaseSignalSlotEventType:
            context BEFORE == m_ObjectCurrentyWithExitSignalInItsOrderedListOfStatements, takeAt + append AFTER, but to BEFORE's context
            context AFTER == m_ObjectCurrentyWithExitSignalInItsOrderedListOfStatements, takeAt + append [after implied]



        OLD: (note: doing both sides of '&&' would waste cpu cycles, but ultimately do nothing. can be guarded against, but KISS TODOoptional)

        TODOreq: signal/slot both on same object (useful as fuck design strat. "hooks"). hell, that req should be noted just for the UML/GUI design of such signal/slot on same object (arched 180 degree arrow pointing to self, auto-generation of "handle*" slot)

        any time append mentioned, index recorded
    */

#if 0
    if(SlotWithCurrentContext == m_ObjectCurrentyWithExitSignalInItsOrderedListOfStatements) //TODOreq: when it's a signal+slot event, we need to check both the signal and the slot, then remove/re-add accordingly. The context may have changed to m_ObjectCurrentyWithExitSignalInItsOrderedListOfStatements after addEventPrivateWithoutUpdatingExitSignal was called, so I think we need to check for removal either again or then/later (unsure).
    {
        //TODO LEFT OFF
        //TODOreq: remove ExitSignal
    }
#endif

    //TODOreq: UML ExitSignal added before any other event, relates to next line kinda
    //TO DOnereq: first event added has no context, so dereferencing current context (TO DOnereq: ExitSignal (m_SlotWithExitSignalCurrentlyInItsOrderedListOfStatements implicitly) may also be 0, so check ExitSignal != 0 before doing any deeper checks) leads to segfault

    DesignEqualsImplementationClassSlot *contextToMoveExitSignalTo = 0;

    if(ExitSignal && SlotWithCurrentContext) //TODOreq: ExitSignal might be added FIRST (so SlotWithCurrentContext might be zero, but ExitSignal is already in Ordered Statement List (or hey, maybe it just segfaults idfk). needs to be moved but won't!). For now, just disable that ability in the GUI. Present them with an error and say they have to add a slot call first. Still, I kinda get why someone would add an ExitSignal first... so I want to fix that behaviour. I _THINK_ the proper solution to this might involve binding the ExitSignal with a class instead of a slot
    {
        //UseCaseSlotEventType and UseCaseSignalSlotEventType (before check)
        if((useCaseEventType == UseCaseSlotEventType || useCaseEventType == UseCaseSignalSlotEventType) && SlotWithCurrentContext->ParentClass == m_SlotWithExitSignalCurrentlyInItsOrderedListOfStatements->ParentClass)
        {
            contextToMoveExitSignalTo = SlotWithCurrentContext;
        }
    }

    /////////////////////BEFORE/////////////////////
    addEventPrivateWithoutUpdatingExitSignal(useCaseEventType, event, signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot);
    /////////////////////AFTER//////////////////////

    if(ExitSignal && SlotWithCurrentContext)
    {
        //UseCaseSlotEventType and UseCaseSignalSlotEventType (after check. the "!contextToMoveExitSignalTo" part of the if is to account for, and not overwrite, the before check
        if((useCaseEventType == UseCaseSlotEventType || useCaseEventType == UseCaseSignalSlotEventType) && !contextToMoveExitSignalTo && SlotWithCurrentContext->ParentClass == m_SlotWithExitSignalCurrentlyInItsOrderedListOfStatements->ParentClass)
        {
            contextToMoveExitSignalTo = SlotWithCurrentContext;
        }

        //UseCaseSignalEventType
        if(useCaseEventType == UseCaseSignalEventType && SlotWithCurrentContext->ParentClass == m_SlotWithExitSignalCurrentlyInItsOrderedListOfStatements->ParentClass)
        {
            contextToMoveExitSignalTo = SlotWithCurrentContext;
        }


        if(contextToMoveExitSignalTo)
        {
            IDesignEqualsImplementationStatement *exitSignalBeingMoved = m_SlotWithExitSignalCurrentlyInItsOrderedListOfStatements->OrderedListOfStatements.takeAt(m_ExitSignalsIndexIntoOrderedListOfStatements);
            m_ExitSignalsIndexIntoOrderedListOfStatements = contextToMoveExitSignalTo->OrderedListOfStatements.size();
            contextToMoveExitSignalTo->OrderedListOfStatements.append(exitSignalBeingMoved);
            m_SlotWithExitSignalCurrentlyInItsOrderedListOfStatements = contextToMoveExitSignalTo; //I get the feeling this won't actually change... which means my remembering of the context 'before' was pointless (i was using both a bool flag "moveContext" in addition to contextToMoveExitSignalTo, but then factored out the moveContext bool). Bleh fuck it, I just want it to work
        }
    }
#endif
}
#if 0
void DesignEqualsImplementationUseCase::addEventPrivateWithoutUpdatingExitSignal(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventType, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot)
{ }
#endif
void DesignEqualsImplementationUseCase::addActorToUseCase(QPointF position)
{
    //TODOreq: either actor or signal-entry point, but not both (error)

    DesignEqualsImplementationActor *actor = new DesignEqualsImplementationActor(position, this);

    //TODOreq: this->setActor or something

    emit actorAdded(actor);
}
//So apparently a life line is a ....... wait no that's not ENTIRELY correct because points can be shared...... named variable [in a different lifeline/class]. So Bar needs to show up as "Bar* Foo::m_Bar", not just "Bar". Because I need the variable name "m_Bar" when adding the slot invocation use case event. I think that Bar can be shared is mostly irrelevant (and I sure hope it is, because it'd require a huge refactor otherwise :-/). The m_Bar variable name is what is relevant to this specific use case.... it is how Foo knows Bar. That other objects in other use cases can have different names for Bar (retrieved however) is irrelevant (except to those use cases/classes ;-P). Foo doesn't just haveA _pointer_ to Bar. Foo hasA Bar itself (that it is a pointer is an implementation detail)
//Foo has no such parent that owns him, so this slot needs to adapt for both! I'm liking the wording "top level object" (Foo is, Bar isn't)
//TODOreq: Additionally, when adding Bar when he's not a member of Foo, I should perhaps query the user and ask them who they want to own Bar. I _THINK_ only the use-case-entry-point-first-slot ParentClass can be a top level object, but I might be wrong here.... that particular statement might only apply to Actor->slotInvoke-as-first-use-case-entry-point use cases.... and not to signal->slot-as-first-use-case-entry-point use cases. I've not thought it through enough, and conceptualizing it is teh difficultz. In any case, Foo still needs to know a name of Bar if Foo only has an initialized-some-time-before-use-case pointer to Bar... if Foo wishes to invoke Bar.
void DesignEqualsImplementationUseCase::addClassToUseCase(DesignEqualsImplementationClass *classToAddToUseCase, HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, QPointF position) //TODOreq: serialize position
{
    //classThatHasMe_OrZeroIfTopLevelObject contains a pointer to classToAddToUseCase in his HasA_PrivateMemberClasses, but we couldn't just pass in a HasA_PrivateMemberClasse because then we couldn't do top level objects (such as the first class invoked in a use case, Foo)

    //TODOreq
    //Weird just realized I haven't even designed "classes" into use cases [yet], as of now use case events point directly to their slots/etc!! But eh the concept of lifelines is derp easy, and the arrows source/destination stuff is really just used in populating which of those signals/slots to use for the already-design (;-D) use-case-event (slot/signal-slot/etc)... but i mean sure there's still a boat load of visual work that needs to be done right about now :-P
    //TODOreq: not sure if front-end or backend should enforce it (or both), but class lifelines should all share the same QPointF::top... just like in Umbrello (Umbrello does some things right, Dia others (like not crashing especially guh allmyrage)). Actor should also utilize that top line, though be slighly below it... AND right above the first lifeline but mb horizontally in between the actor and that first lifeline should go the "Use Case Name" in an oval :-P (but actually ovals take up a ton of space, mb rounded rect (different radius than classes) instead)

    DesignEqualsImplementationClassLifeLine *classLifeLineToAddToUseCase = new DesignEqualsImplementationClassLifeLine(classToAddToUseCase, myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, position, this);

    m_ClassLifeLines.append(classLifeLineToAddToUseCase);

    //Temp:
    emit classLifeLineAdded(classLifeLineToAddToUseCase);
}
//Use Case first-slot entry point and also normal slot invocation from within another slot
void DesignEqualsImplementationUseCase::insertSlotInvocationEvent(int indexToInsertEventAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassLifeLineUnitOfExecution *destinationUnitOfExecution_OrZeroIfDestIsActor, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, const SignalEmissionOrSlotInvocationContextVariables &slotInvocationContextVariables)
{
    insertEventPrivate(UseCaseSlotEventType, indexToInsertEventAt, sourceOrderedListOfStatements_OrZeroIfSourceIsActor, destinationUnitOfExecution_OrZeroIfDestIsActor, designEqualsImplementationClassSlot, slotInvocationContextVariables);
}
//Signal emitted during normal slot execution, slot handler for that signal is relevant to use case
void DesignEqualsImplementationUseCase::insertSignalSlotActivationEvent(int indexToInsertEventAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassLifeLineUnitOfExecution *destinationUnitOfExecution_OrZeroIfDestIsActor, DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionContextVariables)
{
    insertEventPrivate(UseCaseSignalSlotEventType, indexToInsertEventAt, sourceOrderedListOfStatements_OrZeroIfSourceIsActor, destinationUnitOfExecution_OrZeroIfDestIsActor, new SignalSlotCombinedEventHolder(designEqualsImplementationClassSignal, designEqualsImplementationClassSlot), signalEmissionContextVariables);
}
//Signals with no listeners in this use case
void DesignEqualsImplementationUseCase::insertSignalEmitEvent(int indexToInsertEventAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassLifeLineUnitOfExecution *destinationUnitOfExecution_OrZeroIfDestIsActor, DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionContextVariables)
{
    insertEventPrivate(UseCaseSignalEventType, indexToInsertEventAt, sourceOrderedListOfStatements_OrZeroIfSourceIsActor, destinationUnitOfExecution_OrZeroIfDestIsActor, designEqualsImplementationClassSignal, signalEmissionContextVariables);
}
//TODOreq: haven't implemented regular signal/slot event deletion, but still worth noting that ExitSignal deletion needs to be handled differently
//TODOreq: again, somewhat off-topic though. should deleting a signal emission or slot invocation mean that it's existence in the class diagram perspective is also deleted? reference counting might be nice, but auto deletion might piss me off too! maybe on deletion, a reference counter is used to ask the user if they want to delete it's existence in the class too (when reference count drops to zero), and/or of course all kinds of "remember this choice" customizations
//TODOreq: in the UML gui, exit signal should always be visually at the bottom-left of the class emitting it, and the bottom-right of the actor
//TODOreq: a finished or exit signal can be emitted right in the middle of a slot/use-case, why not? However now I shouldn't move it to the "last()" unit of execution whenever more are added (the user would still see that happen and 'approve' it, so maybe it is ok). Right now the exit signal is "the last statement in first slot (the first unit of execution, even though a design might have a gap in class lifeline and show a second unit of execution for that same class (handling a response from a child qobject)
void DesignEqualsImplementationUseCase::setExitSignal(DesignEqualsImplementationClassLifeLineUnitOfExecution *sourceUnitOfExecution, DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, const SignalEmissionOrSlotInvocationContextVariables &exitSignalEmissionContextVariables)
{
    //TODOreq: finished/exit signals don't apply to C++/non-thread-safe mode? for now static (had:dynamic, but shit isn't working yo) casting... so we'll segfault if it isn'tlewl
    if(sourceUnitOfExecution) //should have already filtered out "no signal" and "actor as signal", but just in case....
    {
        DesignEqualsImplementationClassSlot *slotAkaUnitOfExecutionThatWasSourceOfArrow = static_cast<DesignEqualsImplementationClassSlot*>(sourceUnitOfExecution->MethodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution);
        if(slotAkaUnitOfExecutionThatWasSourceOfArrow)
        {
            slotAkaUnitOfExecutionThatWasSourceOfArrow->setFinishedOrExitSignal(designEqualsImplementationClassSignal, exitSignalEmissionContextVariables);
        }
    }

#if 0
    if(ExitSignal)
    {
        //remove it from old context
        IDesignEqualsImplementationStatement *oldExitSignalStatement = m_SlotWithExitSignalCurrentlyInItsOrderedListOfStatements->OrderedListOfStatements.takeAt(m_ExitSignalsIndexIntoOrderedListOfStatements); //TODOreq: any time from now on that m_ObjectCurrentyWithExitSignalInItsOrderedListOfStatements adds a statement, we need to remove the exit signal before that, append it back on after the new statement is added, then update m_ExitSignalsIndexIntoOrderedListOfStatements to reflect that. Basically hacky synchronization xD. There's gotta be a better way, but I can't think of it because there is no UseCase context as of now in Class::generateSourceCode
        //TODOreq: if m_ObjectCurrentyWithExitSignalInItsOrderedListOfStatements is deleted in the UML gui, we need to set ExitSignal to zero too (it's implied that all of his children (statements too) are deleted, BUT that implication does include ZERO'ing out ExitSignal (and must))
        delete oldExitSignalStatement;
    }
    //Add to current context (TODOreq: what if it's the first event? does that even make sense? methinks not, but it might segfault if yes)
    m_SlotWithExitSignalCurrentlyInItsOrderedListOfStatements = SlotWithCurrentContext;
    m_ExitSignalsIndexIntoOrderedListOfStatements = SlotWithCurrentContext->OrderedListOfStatements.size();

    addEventPrivateWithoutUpdatingExitSignal(UseCaseSignalEventType, designEqualsImplementationClassSignal, exitSignalEmissionContextVariables);

    ExitSignal = designEqualsImplementationClassSignal;
#endif
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationUseCase &useCase)
{
    qint32 numOrderedUseCaseEvents = useCase.SemiOrderedUseCaseEvents.size();
    //bool hasExitSignal = useCase.ExitSignal != 0; TODOreq: serialize it now in slot
    DesignEqualsImplementationUseCase_QDS(<<, out, useCase)
    //if(hasExitSignal)
    //    out << *useCase.ExitSignal;
    for(qint32 i = 0; i < numOrderedUseCaseEvents; ++i)
    {
        EventAndTypeTypedef eventAndType = useCase.SemiOrderedUseCaseEvents.at(i);
        DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventType = eventAndType.first;
        out << static_cast<quint8>(useCaseEventType);
        switch(useCaseEventType)
        {
        case DesignEqualsImplementationUseCase::UseCaseSlotEventType:
            out << *(static_cast<DesignEqualsImplementationClassSlot*>(eventAndType.second));
            break;
        case DesignEqualsImplementationUseCase::UseCaseSignalEventType:
            out << *(static_cast<DesignEqualsImplementationClassSignal*>(eventAndType.second));
            break;
        case DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType:
            out << *(static_cast<SignalSlotCombinedEventHolder*>(eventAndType.second));
            break;
        }
    }
    return out;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationUseCase &useCase)
{
    qint32 numOrderedUseCaseEvents;
    //bool hasExitSignal;
    DesignEqualsImplementationUseCase_QDS(>>, in, useCase)
    /*if(hasExitSignal)
    {
        DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal = new DesignEqualsImplementationClassSignal(&useCase);
        in >> *designEqualsImplementationClassSignal;
        useCase.ExitSignal = designEqualsImplementationClassSignal;
    }*/
    for(qint32 i = 0; i < numOrderedUseCaseEvents; ++i)
    {
        quint8 useCaseEventTypeAsQuint8;
        in >> useCaseEventTypeAsQuint8;
        DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventType = static_cast<DesignEqualsImplementationUseCase::UseCaseEventTypeEnum>(useCaseEventTypeAsQuint8);
        switch(useCaseEventType)
        {
        case DesignEqualsImplementationUseCase::UseCaseSlotEventType:
            {
                DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot = new DesignEqualsImplementationClassSlot(&useCase); //TODOreq: weird parenting here and below instantiations also. not class for parent as usual
                in >> *designEqualsImplementationClassSlot;
                useCase.SemiOrderedUseCaseEvents.append(qMakePair(useCaseEventType, designEqualsImplementationClassSlot));
            }
            break;
        case DesignEqualsImplementationUseCase::UseCaseSignalEventType:
            {
                DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal = new DesignEqualsImplementationClassSignal(&useCase);
                in >> *designEqualsImplementationClassSignal;
                useCase.SemiOrderedUseCaseEvents.append(qMakePair(useCaseEventType, designEqualsImplementationClassSignal));
            }
            break;
        case DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType:
            {
                SignalSlotCombinedEventHolder *signalSlotCombinedEventHolder = new SignalSlotCombinedEventHolder(&useCase);
                in >> *signalSlotCombinedEventHolder;
                useCase.SemiOrderedUseCaseEvents.append(qMakePair(useCaseEventType, signalSlotCombinedEventHolder));
            }
            break;
        }
    }
    return in;
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationUseCase *&useCase)
{
    return out << *useCase;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationUseCase *&useCase)
{
    useCase = new DesignEqualsImplementationUseCase(); //TODOreq: no parent at this context (just memory related), also losing project we belong to (but since one project per serialized file, that will be easy to fix)
    return in >> *useCase;
}
QDataStream &operator<<(QDataStream &out, const SignalSlotCombinedEventHolder &useCase)
{
    out << *useCase.m_DesignEqualsImplementationClassSignal;
    out << *useCase.m_DesignEqualsImplementationClassSlot;
    return out;
}
QDataStream &operator>>(QDataStream &in, SignalSlotCombinedEventHolder &useCase)
{
    in >> *useCase.m_DesignEqualsImplementationClassSignal;
    in >> *useCase.m_DesignEqualsImplementationClassSlot;
    return in;
}
