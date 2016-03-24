#ifndef DESIGNEQUALSIMPLEMENTATIONIMPLICITLYSHAREDDATATYPE_H
#define DESIGNEQUALSIMPLEMENTATIONIMPLICITLYSHAREDDATATYPE_H

#include "designequalsimplementationtype.h"

class DesignEqualsImplementationImplicitlySharedDataType : public DesignEqualsImplementationType
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationImplicitlySharedDataType(QObject *parent, DesignEqualsImplementationProject *parentProject);
    void addNonFunctionMember(NonFunctionMember *nonFunctionMember);
    int typeCategory() const { return 1; }
    QString privateImplementationTypeName() const;
signals:
    void nonFunctionMemberAdded(NonFunctionMember *nonFunctionMember);
};

#endif // DESIGNEQUALSIMPLEMENTATIONIMPLICITLYSHAREDDATATYPE_H
