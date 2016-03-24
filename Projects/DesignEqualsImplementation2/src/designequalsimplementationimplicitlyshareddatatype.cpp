#include "designequalsimplementationimplicitlyshareddatatype.h"

#include "designequalsimplementationclass.h"
#include "designequalsimplementationclassproperty.h"

DesignEqualsImplementationImplicitlySharedDataType::DesignEqualsImplementationImplicitlySharedDataType(QObject *parent, DesignEqualsImplementationProject *parentProject)
    : Type(parent, parentProject)
{ }
void DesignEqualsImplementationImplicitlySharedDataType::addNonFunctionMember(NonFunctionMember *nonFunctionMember)
{
    if(qobject_cast<DesignEqualsImplementationClass*>(nonFunctionMember->type))
    {
        qFatal("Tried to add a business/QObject to an implicitly shared data type via addNonFunctionMember(). The caller should have checked this."); //only DesignEqualsImplementationClass_aka_BusinessQObjects can have themselves as children
        return;
    }

    if(qobject_cast<DesignEqualsImplementationClassProperty*>(nonFunctionMember))
    {
        qFatal("Tried to add a Q_PROPERTY to an implicitly shared data type via addNonFunctionMember(). The caller should have checked this."); //only DesignEqualsImplementationClass_aka_BusinessQObjects can have Q_PROPERTY 'members'
        return;
    }

    addNonFunctionMemberPrivate(nonFunctionMember);
}
QString DesignEqualsImplementationImplicitlySharedDataType::privateImplementationTypeName() const
{
    return Name + "Data"; //TODOreq: check against this in member name collisions? wait no it's a type, so in type name collisions (which I don't even have coded atm)
}
