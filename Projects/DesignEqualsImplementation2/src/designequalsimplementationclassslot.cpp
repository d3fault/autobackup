#include "designequalsimplementationclassslot.h"

#include <QDataStream>

#define DesignEqualsImplementationClassSlot_QDS(qds, direction, slot) \
qds direction slot.Name; \
qds direction slot.Arguments; \
return qds;

DesignEqualsImplementationClassSlot::DesignEqualsImplementationClassSlot(QObject *parent)
    : QObject(parent)
    , IDesignEqualsImplementationHaveOrderedListOfStatements()
    , IDesignEqualsImplementationMethod()
{ }
DesignEqualsImplementationClassSlot::~DesignEqualsImplementationClassSlot()
{ }
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
