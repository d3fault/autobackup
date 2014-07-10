#ifndef IDESIGNEQUALSIMPLEMENTATIONMETHOD_H
#define IDESIGNEQUALSIMPLEMENTATIONMETHOD_H

#include <QString>
#include <QList>

class DesignEqualsImplementationClass;
class DesignEqualsImplementationClassMethodArgument;

class IDesignEqualsImplementationMethod
{
public:
    explicit IDesignEqualsImplementationMethod();
    virtual ~IDesignEqualsImplementationMethod();

    //TODOoptional: private + getter/setter blah
    QString Name;
    QList<DesignEqualsImplementationClassMethodArgument*> Arguments;
    DesignEqualsImplementationClass *ParentClass; //TODOreq: [de-]serialization

    QString methodSignatureWithoutReturnType();
    QString argumentsToCommaSeparatedString();
};

#endif // IDESIGNEQUALSIMPLEMENTATIONMETHOD_H
