#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSMETHODARGUMENT_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSMETHODARGUMENT_H

#include "designequalsimplementationtype.h"

class IDesignEqualsImplementationMethod;

//I should change the name to d=iFunctionArgument TODOoptional
class DesignEqualsImplementationClassMethodArgument : public TypeInstance
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassMethodArgument(IDesignEqualsImplementationMethod *parentMethod, DesignEqualsImplementationType *type, const QString &qualifiedTypeString, const QString &variableName, QObject *parent = 0);
    IDesignEqualsImplementationMethod *ParentMethod;
protected:
    TypeInstanceCategory typeInstanceCategory();
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSMETHODARGUMENT_H
