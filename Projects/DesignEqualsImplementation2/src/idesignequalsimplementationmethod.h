#ifndef IDESIGNEQUALSIMPLEMENTATIONMETHOD_H
#define IDESIGNEQUALSIMPLEMENTATIONMETHOD_H

#include <QString>
#include <QList>
#include <QPair>

#include "designequalsimplementationtype.h"
#include "designequalsimplementationclassmethodargument.h"
#include "libclangfunctiondeclarationparser.h"

class DesignEqualsImplementationClass;

class IDesignEqualsImplementationMethod
{
public:
    enum MethodSignatureFlagsEnum
    {
        MethodSignatureForVisualAppearanceContainsArgumentVariableNames
        , MethodSignatureNormalizedAndDoesNotContainArgumentsVariableNames
    };

    DesignEqualsImplementationClassMethodArgument *createNewArgument(DesignEqualsImplementationType *argumentType, const QString &qualifiedArgumentTypeString, const QString &argumentVariableName);

    //TODOoptional: private + getter/setter blah
    QString Name;
    QList<DesignEqualsImplementationClassMethodArgument*> arguments() const;
    QList<ParsedTypeInstance> argumentsAsFunctionArgumentTypedefList() const;
    DesignEqualsImplementationClass *ParentClass;

    QString methodSignatureWithoutReturnType(MethodSignatureFlagsEnum methodSignatureFlagsEnum = MethodSignatureForVisualAppearanceContainsArgumentVariableNames);
    QString argumentsToCommaSeparatedString(MethodSignatureFlagsEnum methodSignatureFlagsEnum = MethodSignatureForVisualAppearanceContainsArgumentVariableNames);

protected:
    virtual QObject *asQObject()=0;
private:
    QList<DesignEqualsImplementationClassMethodArgument*> m_Arguments;
};

#endif // IDESIGNEQUALSIMPLEMENTATIONMETHOD_H
