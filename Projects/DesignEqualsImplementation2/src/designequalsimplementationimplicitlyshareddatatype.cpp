#include "designequalsimplementationimplicitlyshareddatatype.h"

#include "designequalsimplementationclass.h"
#include "designequalsimplementationclassproperty.h"

DesignEqualsImplementationImplicitlySharedDataType::DesignEqualsImplementationImplicitlySharedDataType(QObject *parent)
    : Type(parent)
{ }
bool DesignEqualsImplementationImplicitlySharedDataType::addNonFunctionMember(NonFunctionMember *nonFunctionMember)
{
    if(qobject_cast<DesignEqualsImplementationClass*>(nonFunctionMember->type))
    {
        qFatal("Tried to add a business/QObject to an implicitly shared data type via addNonFunctionMember(). The caller should have checked this.");
        return false; //only DesignEqualsImplementationClass_aka_BusinessQObjects can have themselves as children
    }

    if(qobject_cast<DesignEqualsImplementationClassProperty*>(nonFunctionMember))
    {
        qFatal("Tried to add a Q_PROPERTY to an implicitly shared data type via addNonFunctionMember(). The caller should have checked this.");
        return false; //only DesignEqualsImplementationClass_aka_BusinessQObjects can have Q_PROPERTY 'members'
    }

    m_NonFunctionMembers << nonFunctionMember;
    return true;
}
