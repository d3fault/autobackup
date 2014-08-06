#ifndef HASA_PRIVATE_CLASSES_MEMBER_H
#define HASA_PRIVATE_CLASSES_MEMBER_H

#include "ihavetypeandvariablenameandpreferredtextualrepresentation.h"

class DesignEqualsImplementationClass;
class HasA_Private_Classes_Member : public IHaveTypeAndVariableNameAndPreferredTextualRepresentation
{
public:
    DesignEqualsImplementationClass *m_MyClass;
protected:
    virtual QString typeString();
};

#endif // HASA_PRIVATE_CLASSES_MEMBER_H
