#include "designequalsimplementationusecase.h"

#include <QDataStream>

#include "designequalsimplementationactor.h"
#include "designequalsimplementationclasslifeline.h"
#include "designequalsimplementationslotinvocationstatement.h"
#include "designequalsimplementationsignalemissionstatement.h"

typedef QPair<DesignEqualsImplementationUseCase::UseCaseEventTypeEnum, QObject*> EventAndTypeTypedef;

#define DesignEqualsImplementationUseCase_QDS(direction, qds, useCase) \
qds direction useCase.Name; \
qds direction numOrderedUseCaseEvents; \
qds direction hasExitSignal;

//TODOreq: by far the most difficult class to design/[de-]serialize/generate-from. the rest are just busywork by comparison
//TODOoptimization: class diagram serialized first (although use cases must be factored into that!), serves as a simple library or whatever so that use cases only later have to refer to a signal by id/etc rather than redefine/reserialize the signal/slot/etc every time it is referenced
//TODOreq: since it doesn't have much else use, use this->Name in the comments of the generated source code. the use case entry point slot should say the use case name for readability i suppose. also, if verbose logging is on, then each time a use case entry point is crossed we could output the name...
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
//Overload: Signals with no listeners in this use case
void DesignEqualsImplementationUseCase::addEvent(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal)
{
    addEventPrivate(UseCaseSignalEventType, designEqualsImplementationClassSignal);
}
//Overload: Signal emitted during normal slot execution, slot handler for that signal is relevant to use case
void DesignEqualsImplementationUseCase::addEvent(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionContextVariables)
{
    addEventPrivate(UseCaseSignalSlotEventType, new SignalSlotCombinedEventHolder(designEqualsImplementationClassSignal, designEqualsImplementationClassSlot), signalEmissionContextVariables);
}
//TODOreq: haven't implemented regular signal/slot event deletion, but still worth noting that ExitSignal deletion needs to be handled differently
//TODOreq: again, somewhat off-topic though. should deleting a signal emission or slot invocation mean that it's existence in the class diagram perspective is also deleted? reference counting might be nice, but auto deletion might piss me off too! maybe on deletion, a reference counter is used to ask the user if they want to delete it's existence in the class too (when reference count drops to zero), and/or of course all kinds of "remember this choice" customizations
//TODOreq: in the UML gui, exit signal should always be visually at the bottom-left of the class emitting it, and the bottom-right of the actor
void DesignEqualsImplementationUseCase::setExitSignal(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, const SignalEmissionOrSlotInvocationContextVariables &exitSignalEmissionContextVariables)
{
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
    Q_FOREACH(EventAndTypeTypedef eventAndType, OrderedUseCaseEvents)
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
    SlotWithCurrentContext = 0;
    ExitSignal = 0;
    m_DesignEqualsImplementationProject = project;
}
void DesignEqualsImplementationUseCase::addEventPrivate(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventType, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot)
{
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
}
void DesignEqualsImplementationUseCase::addEventPrivateWithoutUpdatingExitSignal(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventType, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot)
{
    bool firstUseCaseEventAdded = OrderedUseCaseEvents.isEmpty();

    //Add it (mainly used for saving/opening ([de-]serialization))
    OrderedUseCaseEvents.append(qMakePair(useCaseEventType, event));

    //Code generation and context setup
    switch(useCaseEventType)
    {
    case UseCaseSlotEventType: //changes context of next use case event
    {
        DesignEqualsImplementationClassSlot *slotUseCaseEvent = static_cast<DesignEqualsImplementationClassSlot*>(event);
        if(firstUseCaseEventAdded) //first event add = use case entry point (TODOreq: ui sanitization so that a non-slot can never be made first (TODOoptional: file load sanitization so that we don't crash if it does happen (fuckit)))
        {
            //we have no context, but want the first slot to be our context! TODOreq: there is still ui/cli context to consider, in which case we would have a context already at this point. if this project is generating in lib mode, we don't
            //generate slot context/empty-impl from current use case event, then set that slot/context/empty-impl as current context

            if(SlotWithCurrentContext)
            {
                //TODOreq: ui already gave us context, so do nameless-signal/invokeMethod on ui to get to this slot, then just change the context as if we were in lib gen mode (fuck. we aren't even generating right now so..... wtf?)
            }
            else
            {
                SlotWithCurrentContext = slotUseCaseEvent;
            }
        }
        else
        {
            //generate slot context/empty-impl from current use case event, then in current context do nameless-signal or invokeMethod to the slot/context/empty-impl, then set that slot/context/empty-impl as current context

            SlotWithCurrentContext->OrderedListOfStatements.append(new DesignEqualsImplementationSlotInvocationStatement(slotUseCaseEvent, signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot));
            SlotWithCurrentContext = slotUseCaseEvent;
        }
    }
        break;
    case UseCaseSignalEventType: //does not change context of next use case event
    {
        DesignEqualsImplementationClassSignal *signalUseCaseEvent = static_cast<DesignEqualsImplementationClassSignal*>(event);
        //in current context, emit the signal

        SlotWithCurrentContext->OrderedListOfStatements.append(new DesignEqualsImplementationSignalEmissionStatement(signalUseCaseEvent, signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot));
    }
        break;
    case UseCaseSignalSlotEventType: //changes context of next use case event. the only difference from UseCaseSignalSlotEventType and UseCaseSlotEventType is that the signal is a named part of the design for UseCaseSignalSlotEventType, whereas UseCaseSlotEventType either uses invokeMethod (could have been an autogenerated nameless signal, but doesn't matter which)
    {
        //TODOreq: right now is when we need to make a note that the signal would be connected in the slot, and optimally resolving where that connection will be made is going to be a bitch :-P
        SignalSlotCombinedEventHolder *signalSlotCombinedUseCaseEvent = static_cast<SignalSlotCombinedEventHolder*>(event);
        SlotWithCurrentContext->OrderedListOfStatements.append(new DesignEqualsImplementationSignalEmissionStatement(signalSlotCombinedUseCaseEvent->m_DesignEqualsImplementationClassSignal, signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot));
        SlotWithCurrentContext = signalSlotCombinedUseCaseEvent->m_DesignEqualsImplementationClassSlot;
        //TO DOnemb (NOPE BECAUSE [de-]serialization): can maybe delete the SignalSlotCombinedEventHolder here/now, since I think I don't need it anymore
    }
        break;
    }
}
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
void DesignEqualsImplementationUseCase::addClassToUseCase(DesignEqualsImplementationClass *classToAddToUseCase, HasA_PrivateMemberClasses_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, QPointF position) //TODOreq: serialize position
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
void DesignEqualsImplementationUseCase::addSlotInvocationEvent(DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, const SignalEmissionOrSlotInvocationContextVariables &slotInvocationContextVariables)
{
    addEventPrivate(UseCaseSlotEventType, designEqualsImplementationClassSlot, slotInvocationContextVariables);
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationUseCase &useCase)
{
    qint32 numOrderedUseCaseEvents = useCase.OrderedUseCaseEvents.size();
    bool hasExitSignal = useCase.ExitSignal != 0;
    DesignEqualsImplementationUseCase_QDS(<<, out, useCase)
    if(hasExitSignal)
        out << *useCase.ExitSignal;
    for(qint32 i = 0; i < numOrderedUseCaseEvents; ++i)
    {
        EventAndTypeTypedef eventAndType = useCase.OrderedUseCaseEvents.at(i);
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
    bool hasExitSignal;
    DesignEqualsImplementationUseCase_QDS(>>, in, useCase)
    if(hasExitSignal)
    {
        DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal = new DesignEqualsImplementationClassSignal(&useCase);
        in >> *designEqualsImplementationClassSignal;
        useCase.ExitSignal = designEqualsImplementationClassSignal;
    }
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
                useCase.OrderedUseCaseEvents.append(qMakePair(useCaseEventType, designEqualsImplementationClassSlot));
            }
            break;
        case DesignEqualsImplementationUseCase::UseCaseSignalEventType:
            {
                DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal = new DesignEqualsImplementationClassSignal(&useCase);
                in >> *designEqualsImplementationClassSignal;
                useCase.OrderedUseCaseEvents.append(qMakePair(useCaseEventType, designEqualsImplementationClassSignal));
            }
            break;
        case DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType:
            {
                SignalSlotCombinedEventHolder *signalSlotCombinedEventHolder = new SignalSlotCombinedEventHolder(&useCase);
                in >> *signalSlotCombinedEventHolder;
                useCase.OrderedUseCaseEvents.append(qMakePair(useCaseEventType, signalSlotCombinedEventHolder));
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
