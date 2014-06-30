#include "designequalsimplementationclasssignal.h"

DesignEqualsImplementationClassSignal::DesignEqualsImplementationClassSignal(QObject *parent)
    : QObject(parent)
{ }
DesignEqualsImplementationClassSignal::~DesignEqualsImplementationClassSignal()
{
    qDeleteAll(Arguments);
}
