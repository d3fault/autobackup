#ifndef DESIGNEQUALSIMPLEMENTATIONIMPLICITLYSHAREDDATATYPE_H
#define DESIGNEQUALSIMPLEMENTATIONIMPLICITLYSHAREDDATATYPE_H

#include "type.h"

class DesignEqualsImplementationImplicitlySharedDataType : public Type
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationImplicitlySharedDataType(QObject *parent = 0);
    void addNonFunctionMember(NonFunctionMember *nonFunctionMember);
    int typeCategory() const { return 1; }
signals:
    void nonFunctionMemberAdded(NonFunctionMember *nonFunctionMember);
};

#endif // DESIGNEQUALSIMPLEMENTATIONIMPLICITLYSHAREDDATATYPE_H
