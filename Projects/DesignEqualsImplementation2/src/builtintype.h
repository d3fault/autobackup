#ifndef BUILTINTYPE_H
#define BUILTINTYPE_H

#include "designequalsimplementationtype.h"

class BuiltInType : public DefinedElsewhereType // : public DesignEqualsImplementationType
{
    Q_OBJECT
public:
    explicit BuiltInType(QObject *parent, DesignEqualsImplementationProject *parentProject)
        : DefinedElsewhereType(parent, parentProject)
    { }
    int typeCategory() const
    {
        return 3;
    }
signals:
    void nonFunctionMemberAdded(NonFunctionMember *nonFunctionMember);
};

#endif // BUILTINTYPE_H
