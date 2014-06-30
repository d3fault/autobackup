#include "designequalsimplementationclassprivatemethod.h"

DesignEqualsImplementationClassPrivateMethod::DesignEqualsImplementationClassPrivateMethod(QObject *parent)
    : QObject(parent)
{ }
DesignEqualsImplementationClassPrivateMethod::~DesignEqualsImplementationClassPrivateMethod()
{
    qDeleteAll(Arguments);
}
