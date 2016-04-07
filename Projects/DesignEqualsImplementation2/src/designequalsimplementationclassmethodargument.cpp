#include "designequalsimplementationclassmethodargument.h"

#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclassprivatemethod.h"

DesignEqualsImplementationClassMethodArgument::DesignEqualsImplementationClassMethodArgument(IDesignEqualsImplementationMethod *parentMethod, DesignEqualsImplementationType *type, const QString &qualifiedTypeString, const QString &variableName, QObject *parent)
    : TypeInstance(type, qualifiedTypeString, variableName, parent)
    , ParentMethod(parentMethod)
{ }
TypeInstance::TypeInstanceCategory DesignEqualsImplementationClassMethodArgument::typeInstanceCategory()
{
    if(qobject_cast<DesignEqualsImplementationClassSlot*>(ParentMethod->asQObject()))
        return SlotArgumentTypeInstance;
    else if(qobject_cast<DesignEqualsImplementationClassPrivateMethod*>(ParentMethod->asQObject()))
        return PrivateMethodArgumentTypeInstance;
    qFatal("Couldn't decide on a type instance category");
    return SlotArgumentTypeInstance; //just so the compiler stfus
}
