#include "designequalsimplementationclassslot.h"

#include <QDataStream>

#define DesignEqualsImplementationClassSlot_QDS(qds, direction, slot) \
qds direction slot.Name; \
qds direction slot.Arguments; \
return qds;

//NOTE: To say all of OrderedListOfStatements represents a single unit of execution is true only when it applies to a slot. The private members things could call other public methods on other objects and still be in the same unit of execution. I guess my design is falling apart when I try to (want to) incorporate the signals-slots thread-safe everywhere uml design method AND the vanilla C++ drop-down uml design method. Perhaps I should just stick to one for now, but blah
//Also, as of now slot has no knowledge of class lifeline or unit of execution... so.... meh. And I'm really not sure I want it to (also not sure I don't want it to)
DesignEqualsImplementationClassSlot::DesignEqualsImplementationClassSlot(QObject *parent)
    : QObject(parent)
    , IDesignEqualsImplementationHaveOrderedListOfStatements()
    , IDesignEqualsImplementationMethod()
    , m_FinishedOrExitSignal(0)
{ }
DesignEqualsImplementationClassSlot::~DesignEqualsImplementationClassSlot()
{ }
void DesignEqualsImplementationClassSlot::setFinishedOrExitSignal(DesignEqualsImplementationClassSignal *finishedOrExitSignal, SignalEmissionOrSlotInvocationContextVariables exitSignalEmissionContextVariables)
{
    m_FinishedOrExitSignal = finishedOrExitSignal;
    m_ExitSignalEmissionContextVariables = exitSignalEmissionContextVariables;
}
DesignEqualsImplementationClassSignal *DesignEqualsImplementationClassSlot::finishedOrExitSignal() const
{
    return m_FinishedOrExitSignal;
}
SignalEmissionOrSlotInvocationContextVariables DesignEqualsImplementationClassSlot::finishedOrExitSignalEmissionContextVariables() const
{
    return m_ExitSignalEmissionContextVariables;
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassSlot &slot)
{
    DesignEqualsImplementationClassSlot_QDS(out, <<, slot)
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSlot &slot)
{
    DesignEqualsImplementationClassSlot_QDS(in, >>, slot)
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassSlot *&slot)
{
    return out << *slot;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSlot *&slot)
{
    slot = new DesignEqualsImplementationClassSlot();
    return in >> *slot;
}
