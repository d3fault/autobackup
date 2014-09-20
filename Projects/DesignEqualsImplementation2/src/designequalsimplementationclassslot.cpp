#include "designequalsimplementationclassslot.h"

#include <QDataStream>

#include "designequalsimplementationproject.h"
#include "designequalsimplementationclass.h"
#include "designequalsimplementationclasslifeline.h"

#define DesignEqualsImplementationClassSlot_QDS(qds, direction, slot) \
qds direction slot.Name; \
qds direction slot.m_Arguments; \
qds direction slot.m_OrderedListOfStatements; \
return qds;

//NOTE: To say all of OrderedListOfStatements represents a single unit of execution is true only when it applies to a slot. The private members things could call other public methods on other objects and still be in the same unit of execution. I guess my design is falling apart when I try to (want to) incorporate the signals-slots thread-safe everywhere uml design method AND the vanilla C++ drop-down uml design method. Perhaps I should just stick to one for now, but blah
//Also, as of now slot has no knowledge of class lifeline or unit of execution... so.... meh. And I'm really not sure I want it to (also not sure I don't want it to)
DesignEqualsImplementationClassSlot::DesignEqualsImplementationClassSlot(QObject *parent)
    : QObject(parent)
    , IDesignEqualsImplementationHaveOrderedListOfStatements()
    , IDesignEqualsImplementationMethod()
    //, m_ParentClassLifeLineInUseCaseView_OrZeroInClassDiagramView(0)
    , m_FinishedOrExitSignal(0)
{ }
DesignEqualsImplementationClassSlot::DesignEqualsImplementationClassSlot(DesignEqualsImplementationClassLifeLine *parentClassLifeLineInUseCaseView_OrZeroInClassDiagramView, QObject *parent)
    : QObject(parent)
    , IDesignEqualsImplementationHaveOrderedListOfStatements()
    , IDesignEqualsImplementationMethod()
    //, m_ParentClassLifeLineInUseCaseView_OrZeroInClassDiagramView(parentClassLifeLineInUseCaseView_OrZeroInClassDiagramView) //class lifeline tells us our variable name should we need it (as seen, we do). it also can/will tell us if we are a top level object or if we have a parent object that hasA us
    , m_FinishedOrExitSignal(0)
{
    if(parentClassLifeLineInUseCaseView_OrZeroInClassDiagramView)
    {
        ParentClass = parentClassLifeLineInUseCaseView_OrZeroInClassDiagramView->designEqualsImplementationClass();
    }
}
DesignEqualsImplementationClassSlot::~DesignEqualsImplementationClassSlot()
{
    if(ParentClass)
        ParentClass->removeSlot(this);
}
#if 0
void DesignEqualsImplementationClassSlot::setParentClassLifeLineInUseCaseView_OrZeroInClassDiagramView_OrZeroWhenFirstTimeSlotIsUsedInAnyUseCaseInTheProject(DesignEqualsImplementationClassLifeLine *parentClassLifeLine)
{
    m_ParentClassLifeLineInUseCaseView_OrZeroInClassDiagramView = parentClassLifeLine;
}
DesignEqualsImplementationClassLifeLine* DesignEqualsImplementationClassSlot::parentClassLifeLineInUseCaseView_OrZeroInClassDiagramView_OrZeroWhenFirstTimeSlotIsUsedInAnyUseCaseInTheProject() const
{
    return m_ParentClassLifeLineInUseCaseView_OrZeroInClassDiagramView;
}
#endif
//TODOreq: exit signal makes more sense for an entire USE CASE, which means that the signal might be emitted from a different slot (on same class lifeline) after some async backend object operation (on other class lifeline). I know that I do have smart re-ordering logic around the exit signal (so that it is always 'last' statement), and that logic kind of conflicts with what I just said about exit signal being property of the USE CASE instead of a single slot. BUT OTOH, a slot is a use case [entry point], and especially when using a slot as a 'slot reference' from some other use case, we would still want the "exit signal" to be available to that use case using said slot reference, so even if it is emitted from a different ACTUAL slot, it is still KIND OF the exit signal for a SLOT, but only because we are viewing slots and use cases as sort of the same thing. But, however, how I currently have it coded is that the exit signal is assumed to be a statement of the current slot, whereas it may be a statement of a later-triggered-asynchronously slot. At a glance that might be easily changeable since I'm passing a pointer to a signal around and calling it the exit signal. I guess really this indirectly depends on whatever design I come up with to implement the "slot REFERENCE in use case" capability.
void DesignEqualsImplementationClassSlot::setFinishedOrExitSignal(DesignEqualsImplementationClassSignal *finishedOrExitSignal, SignalEmissionOrSlotInvocationContextVariables exitSignalEmissionContextVariables)
{
    m_FinishedOrExitSignal = finishedOrExitSignal;
    m_ExitSignalEmissionContextVariables = exitSignalEmissionContextVariables;
}
DesignEqualsImplementationClassSignal *DesignEqualsImplementationClassSlot::finishedOrExitSignal_OrZeroIfNone() const
{
    return m_FinishedOrExitSignal;
}
SignalEmissionOrSlotInvocationContextVariables DesignEqualsImplementationClassSlot::finishedOrExitSignalEmissionContextVariables() const
{
    return m_ExitSignalEmissionContextVariables;
}
QObject *DesignEqualsImplementationClassSlot::asQObject()
{
    return this;
}
DesignEqualsImplementationClassSlot* DesignEqualsImplementationClassSlot::streamInSlotReference(DesignEqualsImplementationProject *project, QDataStream &in)
{
    int classIdOfClassTheSlotIsIn;
    in >> classIdOfClassTheSlotIsIn;
    int slotId;
    in >> slotId;
    DesignEqualsImplementationClass *classTheSlotIsIn = project->classInstantiationFromSerializedClassId(classIdOfClassTheSlotIsIn);
    return classTheSlotIsIn->slotInstantiationFromSerializedSlotId(slotId);
}
void DesignEqualsImplementationClassSlot::streamOutSlotReference(DesignEqualsImplementationProject *project, DesignEqualsImplementationClassSlot *slot, QDataStream &out) //TODOoptional: clean up other slot id uses to use this static method as well
{
    //if we only stream out the slot id, we won't know which class it came from, so we need to stream out the class id too
    DesignEqualsImplementationClass *classTheSlotIsIn = slot->ParentClass;
    out << project->serializationClassIdForClass(classTheSlotIsIn);
    out << classTheSlotIsIn->serializationSlotIdForSlot(slot);
}
#if 0
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassSlot &slot)
{
    DesignEqualsImplementationClassSlot_QDS(out, <<, slot)
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSlot &slot)
{
    DesignEqualsImplementationClassSlot_QDS(in, >>, slot)
}
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassSlot *slot)
{
    out << *slot;
    return out;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSlot *slot)
{
    slot = new DesignEqualsImplementationClassSlot();
    in >> *slot;
    return in;
}
#endif
