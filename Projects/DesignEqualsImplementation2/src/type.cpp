#include "type.h"

//tl;dr: using C++ to model C++. My brain is full of fuck and oh it feels so good [man]
NonFunctionMember *Type::createNewNonFunctionMember(Type *typeOfNewNonFunctionMember, const QString &nameOfNewNonFunctionMember, Visibility::VisibilityEnum visibility, TypeInstanceOwnershipOfPointedToDataIfPointer::TypeInstanceOwnershipOfPointedToDataIfPointerEnum ownershipOfPointedToDataIfPointer, bool hasInit, const QString &optionalInit)
{
    //TODOreq: auto variable name if left blank, just like privateHasa used to
    NonFunctionMember *nonFunctionMember = new NonFunctionMember(typeOfNewNonFunctionMember, nameOfNewNonFunctionMember, this, this, hasInit, optionalInit); //TODOmb: private constructor + friend class Type; , so only Type can new NonFunctionMember
    nonFunctionMember->OwnershipOfPointedTodataIfPointer = ownershipOfPointedToDataIfPointer;
    nonFunctionMember->visibility = visibility;
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
bool TypeInstance::isPointer() const
{
    switch(OwnershipOfPointedTodataIfPointer)
    {
    case TypeInstanceOwnershipOfPointedToDataIfPointer::DoesNotOwnPointedToData:
    case TypeInstanceOwnershipOfPointedToDataIfPointer::OwnsPointedToData:
        return true;
    break;
    case TypeInstanceOwnershipOfPointedToDataIfPointer::NotPointer:
        return false;
    break;
    //the lack of a default: means that if we add to the enum, we'll be forced to update this method :)
    }
    //TODOmb: analyze Qualifiers_RHS for an asterisk???
}
int Type::serializationNonFunctionMemberIdForNonFunctionMember(NonFunctionMember *nonFunctionMember) const
{
    return m_NonFunctionMembers.indexOf(nonFunctionMember);
}
NonFunctionMember *Type::nonFunctionMemberFromNonFunctionMemberId(int nonFunctionMemberId) const
{
    return m_NonFunctionMembers.at(nonFunctionMemberId);
}
QString Type::headerFilenameOnly() const
{
    return Name.toLower() + ".h"; //TODOreq: we don't want int.h, bool.h, etc (same with .cpp). Ideally a type can/does specify N headers it requires. Our DesignEqualsImplementationClass, however, does just use type.name.toLower() ofc...
}
QString Type::sourceFilenameOnly() const
{
    return Name.toLower() + ".cpp";
}
