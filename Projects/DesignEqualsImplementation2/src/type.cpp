#include "type.h"

#include <QStringList>

//tl;dr: using C++ to model C++. My brain is full of fuck and oh it feels so good [man]
NonFunctionMember *Type::createNewNonFunctionMember(Type *typeOfNewNonFunctionMember, const QString &qualifiedTypeString, const QString &nameOfNewNonFunctionMember, Visibility::VisibilityEnum visibility, TypeInstanceOwnershipOfPointedToDataIfPointer::TypeInstanceOwnershipOfPointedToDataIfPointerEnum ownershipOfPointedToDataIfPointer, bool hasInit, const QString &optionalInit)
{
    //TODOreq: auto variable name if left blank, just like privateHasa used to
    NonFunctionMember *nonFunctionMember = new NonFunctionMember(typeOfNewNonFunctionMember, qualifiedTypeString, nameOfNewNonFunctionMember, this, this, hasInit, optionalInit); //TODOmb: private constructor + friend class Type; , so only Type can new NonFunctionMember
    nonFunctionMember->OwnershipOfPointedTodataIfPointer = ownershipOfPointedToDataIfPointer;
    nonFunctionMember->visibility = visibility;
    addNonFunctionMember(nonFunctionMember); //TODOoptional: the bool it returns is dumb. the qFatals are plenty, so change bool to void
    return nonFunctionMember;
}
QString TypeInstance::preferredTextualRepresentationOfTypeAndVariableTogether(const QString &qualifiedType, const QString &variableName)
{
    if(qualifiedType.endsWith("&") || qualifiedType.endsWith("*")) //etc
    {
        return (qualifiedType + variableName);
    }
    else
    {
        return (qualifiedType + " " + variableName);
    }
}
//ehh a bit hacky but gets the job done
void TypeInstance::parseQualifiedTypeString(const QString &qualifiedTypeString)
{
    QStringList split = qualifiedTypeString.split(type->Name);
    if(split.size() != 2)
    {
        qFatal("qualified type did not have type in it");
        return;
    }
    Qualifiers_LHS = split.first();
    Qualifiers_RHS = split.last();
    //TODOoptional: LHS should be left-trimmed, RHS should be right-trimmed, but we want to keep the whitespace "on the variableName side(s)" intact
}
QString TypeInstance::nonQualifiedType() const
{
    return type->Name;
}
QString TypeInstance::qualifiedType() const
{
    return QString(Qualifiers_LHS + type->Name + Qualifiers_RHS);
}
QString TypeInstance::preferredTextualRepresentationOfTypeAndVariableTogether() const
{
    return preferredTextualRepresentationOfTypeAndVariableTogether(qualifiedType(), VariableName);
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
    //TODOmb: analyze Qualifiers_RHS for an asterisk??? if so, should probably do it during parseQualifiedTypeString and then set the enum accordingly (default to non-owner pointer when asterisk seen, ofc)
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
void Type::addNonFunctionMemberPrivate(NonFunctionMember *nonFunctionMember)
{
    m_NonFunctionMembers << nonFunctionMember;
    /*emit */nonFunctionMemberAdded(nonFunctionMember);
}
