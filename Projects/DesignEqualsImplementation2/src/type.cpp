#include "type.h"

NonFunctionMember *Type::createNewNonFunctionMember(Type *typeOfNewNonFunctionMember, const QString &nameOfNewNonFunctionMember, Visibility::VisibilityEnum visibility, NonFunctionMemberOwnershipOfPointedToDataIfPointer::NonFunctionMemberOwnershipOfPointedToDataIfPointerEnum ownershipOfPointedToDataIfPointer)
{
    //TODOreq: auto variable name if left blank, just like privateHasa used to
    NonFunctionMember *nonFunctionMember = new NonFunctionMember(typeOfNewNonFunctionMember, nameOfNewNonFunctionMember, this); //TODOmb: private constructor + friend class Type; , so only Type can new NonFunctionMember
    nonFunctionMember->OwnershipOfPointedTodataIfPointer = ownershipOfPointedToDataIfPointer;
    nonFunctionMember->visibility = visibility;
    //nonFunctionMember.typeInstance = new TypeInstance(non)
    addNonFunctionMember(nonFunctionMember); //TODOoptional: the bool it returns is dumb. the qFatals are plenty, so change bool to void
    return nonFunctionMember;
}
QString TypeInstance::preferredTextualRepresentationOfTypeAndVariableTogether() const
{
    QString ret(Qualifiers_LHS + type->Name + Qualifiers_RHS); //TODOreq: actually use lhs/rhs aside from just here
    if(ret.endsWith("&") || ret.endsWith("*")) //etc
    {
        return (ret + VariableName);
    }
    else
    {
        return (ret + " " + VariableName);
    }
}
