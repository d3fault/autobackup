#include "designequalsimplementationclassslot.h"

DesignEqualsImplementationClassSlot::DesignEqualsImplementationClassSlot(QObject *parent)
    : QObject(parent)
{ }
DesignEqualsImplementationClassSlot::~DesignEqualsImplementationClassSlot()
{
    qDeleteAll(Arguments);
}
