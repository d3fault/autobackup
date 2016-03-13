#ifndef HASA_PRIVATE_CLASSES_MEMBER_H
#define HASA_PRIVATE_CLASSES_MEMBER_H
#if 1

#include "ihavetypeandvariablenameandpreferredtextualrepresentation.h"

class DesignEqualsImplementationClass;
class HasA_Private_Classes_Member : public IHaveTypeAndVariableNameAndPreferredTextualRepresentation
{
public:
    DesignEqualsImplementationClass *m_MyClass;

    DesignEqualsImplementationClass *parentClass() const;
    void setParentClass(DesignEqualsImplementationClass *parentClass);
public:
    DesignEqualsImplementationClass *m_ParentClass;
protected:
    virtual QString typeString();
};

QDataStream &operator<<(QDataStream &out, HasA_Private_Classes_Member &hasA_Private_Classes_Member);
QDataStream &operator>>(QDataStream &in, HasA_Private_Classes_Member &hasA_Private_Classes_Member);
QDataStream &operator<<(QDataStream &out, HasA_Private_Classes_Member *hasA_Private_Classes_Member);
QDataStream &operator>>(QDataStream &in, HasA_Private_Classes_Member *hasA_Private_Classes_Member);


Q_DECLARE_METATYPE(HasA_Private_Classes_Member*)

#endif
#endif // HASA_PRIVATE_CLASSES_MEMBER_H
