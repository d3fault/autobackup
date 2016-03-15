#ifndef DESIGNEQUALSIMPLEMENTATIONIMPLICITLYSHAREDDATATYPE_H
#define DESIGNEQUALSIMPLEMENTATIONIMPLICITLYSHAREDDATATYPE_H

#include "type.h"

class DesignEqualsImplementationImplicitlySharedDataType : public Type
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationImplicitlySharedDataType(QObject *parent = 0);
    bool addNonFunctionMember(NonFunctionMember *nonFunctionMember);
    int typeType() const { return 1; }
};

#endif // DESIGNEQUALSIMPLEMENTATIONIMPLICITLYSHAREDDATATYPE_H
