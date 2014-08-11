#ifndef IDESIGNEQUALSIMPLEMENTATIONMETHOD_H
#define IDESIGNEQUALSIMPLEMENTATIONMETHOD_H

#include <QString>
#include <QList>

#include "designequalsimplementationclassmethodargument.h"

class DesignEqualsImplementationClass;
class DesignEqualsImplementationClassMethodArgument;

class IDesignEqualsImplementationMethod
{
public:
    enum MethodSignatureFlagsEnum
    {
        MethodSignatureForVisualAppearanceContainsArgumentVariableNames
        , MethodSignatureNormalizedAndDoesNotContainArgumentsVariableNames
    };

    explicit IDesignEqualsImplementationMethod();
    virtual ~IDesignEqualsImplementationMethod();

    DesignEqualsImplementationClassMethodArgument *createNewArgument(const QString &argumentType = QString(), const QString &argumentVariableName = QString());

    //TODOoptional: private + getter/setter blah
    QString Name;
    QList<DesignEqualsImplementationClassMethodArgument*> arguments();
    DesignEqualsImplementationClass *ParentClass; //TODOreq: [de-]serialization

    QString methodSignatureWithoutReturnType(MethodSignatureFlagsEnum methodSignatureFlagsEnum = MethodSignatureForVisualAppearanceContainsArgumentVariableNames);
    QString argumentsToCommaSeparatedString(MethodSignatureFlagsEnum methodSignatureFlagsEnum = MethodSignatureForVisualAppearanceContainsArgumentVariableNames);

    //TODOoptional: should be private:
    QList<DesignEqualsImplementationClassMethodArgument*> m_Arguments;
};

#endif // IDESIGNEQUALSIMPLEMENTATIONMETHOD_H
