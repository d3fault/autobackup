#include "designequalsimplementationclass.h"

DesignEqualsImplementationClass::DesignEqualsImplementationClass(QObject *parent)
    : QObject(parent)
{ }
bool DesignEqualsImplementationClass::generateSourceCode(const QString &destinationDirectoryPath)
{
    //TODOreq
    return true;
}
DesignEqualsImplementationClass::~DesignEqualsImplementationClass()
{
    qDeleteAll(Properties);
    qDeleteAll(PrivateMethods);
    qDeleteAll(Slots);
    qDeleteAll(Signals);
}
