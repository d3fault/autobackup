#ifndef IDESIGNEQUALSIMPLEMENTATIONMETHOD_H
#define IDESIGNEQUALSIMPLEMENTATIONMETHOD_H

#include <QString>
#include <QList>

class DesignEqualsImplementationClassMethodArgument;

class IDesignEqualsImplementationMethod
{
public:
    explicit IDesignEqualsImplementationMethod();
    virtual ~IDesignEqualsImplementationMethod();

    //TODOoptional: private + getter/setter blah
    QString Name;
    QList<DesignEqualsImplementationClassMethodArgument*> Arguments;

    QString methodSignatureWithoutReturnType();
    QString argumentsToCommaSeparatedString();
};

#endif // IDESIGNEQUALSIMPLEMENTATIONMETHOD_H
