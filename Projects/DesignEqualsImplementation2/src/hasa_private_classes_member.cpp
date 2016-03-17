#include "hasa_private_classes_member.h"
#if 1

#include <QDataStream>

#include "designequalsimplementationclass.h"
#include "designequalsimplementationproject.h"

DesignEqualsImplementationClass *HasA_Private_Classes_Member::parentClass() const
{
    return m_ParentClass;
}
void HasA_Private_Classes_Member::setParentClass(DesignEqualsImplementationClass *parentClass)
{
    m_ParentClass = parentClass;
}
QString HasA_Private_Classes_Member::typeString()
{
    return m_MyClass->Name + " *";
}
#endif
