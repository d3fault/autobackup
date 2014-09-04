#include "hasa_private_classes_member.h"

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
    return m_MyClass->ClassName + " *";
}
QDataStream &operator<<(QDataStream &out, HasA_Private_Classes_Member &hasA_Private_Classes_Member)
{
    //HasA_Private_Classes_Member_QDS(out, <<, hasA_Private_Classes_Member);
#if 0
#define HasA_Private_Classes_Member_QDS(qds, direction, hasA_Private_Classes_Member) \
qds direction hasA_Private_Classes_Member.m_MyClass->m_ParentProject->serializationClassIdForClass(hasA_Private_Classes_Member.m_MyClass); \
qds direction hasA_Private_Classes_Member.VariableName; \
qds direction hasA_Private_Classes_Member.m_ParentClass->m_ParentProject->serializationClassIdForClass(hasA_Private_Classes_Member.m_ParentClass); \
return qds;
#endif
    //not serialized, but SET just before deserializing because the class hasA it, duh. out << hasA_Private_Classes_Member.m_MyClass->m_ParentProject->serializationClassIdForClass(hasA_Private_Classes_Member.m_MyClass);
    out << hasA_Private_Classes_Member.VariableName;
    out << hasA_Private_Classes_Member.m_ParentClass->m_ParentProject->serializationClassIdForClass(hasA_Private_Classes_Member.m_ParentClass);
}
QDataStream &operator>>(QDataStream &in, HasA_Private_Classes_Member &hasA_Private_Classes_Member)
{
    //HasA_Private_Classes_Member_QDS(in, >>, hasA_Private_Classes_Member);

    //int deserializationClassIdForClass;
    //in >> deserializationClassIdForClass;

    in >> hasA_Private_Classes_Member.VariableName;
    int deserializationClassIdForParentClass;
    in >> deserializationClassIdForParentClass;
    hasA_Private_Classes_Member.m_ParentClass = hasA_Private_Classes_Member.m_MyClass->m_ParentProject->classInstantiationFromSerializedClassId(deserializationClassIdForParentClass);
}
QDataStream &operator<<(QDataStream &out, HasA_Private_Classes_Member *hasA_Private_Classes_Member)
{
    out << *hasA_Private_Classes_Member;
    return out;
}
QDataStream &operator>>(QDataStream &in, HasA_Private_Classes_Member *hasA_Private_Classes_Member)
{
    hasA_Private_Classes_Member = new HasA_Private_Classes_Member();
    in >> *hasA_Private_Classes_Member;
    return in;
}
