#include "type.h"

#include <QStringList>

#include "designequalsimplementationprojectgenerator.h"
#include "designequalsimplementationclass.h"
#include "designequalsimplementationclassproperty.h"

//tl;dr: using C++ to model C++. My brain is full of fuck and oh it feels so good [man]
NonFunctionMember *Type::createNewNonFunctionMember(Type *typeOfNewNonFunctionMember, const QString &qualifiedTypeString, const QString &nameOfNewNonFunctionMember_OrEmptyStringToAutoGenerateOne, Visibility::VisibilityEnum visibility, TypeInstanceOwnershipOfPointedToDataIfPointer::TypeInstanceOwnershipOfPointedToDataIfPointerEnum ownershipOfPointedToDataIfPointer, bool hasInit, const QString &optionalInit)
{
    QString chosenVariableName = nameOfNewNonFunctionMember_OrEmptyStringToAutoGenerateOne;
    if(chosenVariableName.trimmed().isEmpty())
    {
        chosenVariableName = autoNameForNewChildMemberOfType(typeOfNewNonFunctionMember->Name);
    }

    NonFunctionMember *nonFunctionMember = new NonFunctionMember(typeOfNewNonFunctionMember, qualifiedTypeString, chosenVariableName, this, this, hasInit, optionalInit); //TODOmb: private constructor + friend class Type; , so only Type can new NonFunctionMember
    nonFunctionMember->OwnershipOfPointedTodataIfPointer = ownershipOfPointedToDataIfPointer;
    nonFunctionMember->visibility = visibility;
    addNonFunctionMember(nonFunctionMember); //TODOoptional: the bool it returns is dumb. the qFatals are plenty, so change bool to void
    return nonFunctionMember;
}
//TODOreq: anything/everything that is checked against IN this method, should also check against this method when setting it's corresponding 'name'. idk whether it should be in the GUI (unacceptable dialogs, etc) or in the business logic or both or what, but all I know is that collisions [can] happen both ways and right now I'm only checking for collisions when doing 'autoName' shit for NonFunctionMember creation -- it can be put off as low priority since the C++ compiler will catch it for me :)
bool Type::memberWithNameExists(const QString &memberNameToCheckForCollisions) const
{
    //TO DOnemb: for improved readibility (sanity), perhaps I should do case insensitive matching (.toLower both sides)
    QString memberLower = memberNameToCheckForCollisions.toLower();
    Q_FOREACH(NonFunctionMember *currentNonFunctionMember, nonFunctionMembers())
    {
        if(currentNonFunctionMember->VariableName.toLower() == memberLower)
            return true;

        if(DesignEqualsImplementationClassProperty *memberAsProperty = qobject_cast<DesignEqualsImplementationClassProperty*>(currentNonFunctionMember))
        {
            //Q_PROPERTY name/getters/setters/notifiers collision checking
            if(memberAsProperty->propertyName().toLower() == memberLower)
                return true; //this probably isn't strictly speaking a collision, I think property name is used ONLY in the Meta-Object System. The getter can and often does share the property name, but doesn't have to
            if(DesignEqualsImplementationProjectGenerator::getterNameForProperty(memberAsProperty->propertyName()).toLower() == memberLower)
                return true;
            if(!memberAsProperty->ReadOnly) //TODOreq: setting ReadOnly to false later on should again check for collisions? fuck my brain just exploded. maybe it's safer to keep the setter/notifier membernames as 'reserved' regardless of whether or not they are used
            {
                if(DesignEqualsImplementationProjectGenerator::setterNameForProperty(memberAsProperty->propertyName()).toLower() == memberLower)
                    return true;
            }
            if(memberAsProperty->NotifiesOnChange)
            {
                if(DesignEqualsImplementationProjectGenerator::changedSignalForProperty(memberAsProperty->propertyName()).toLower() == memberLower)
                    return true;
            }
        }
    }
    if(DesignEqualsImplementationClass *typeAsClass = qobject_cast<DesignEqualsImplementationClass*>(this))
    {
        //signals/slots
        Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, typeAsClass->mySignals())
        {
            if(currentSignal->Name.toLower() == memberLower)
                return true;
        }
        Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, typeAsClass->mySlots())
        {
            if(currentSlot->Name.toLower() == memberLower)
                return true;
        }
    }
    return false;
}
//TODOblah: can't normal NonFunctionMembers have getters and setters? well obviously yes. I guess atm I'm saying "NonFunctionMembers are their visibility ONLY, if you want getters/setters, use Q_PROPERTY". if I ever refactor to allow NonFunctionMembers to have getters/setters, I need to also search for those collisions in autoNameForNewChildOfType
QString Type::autoNameForNewChildMemberOfType(Type *childMemberClassType) const //TODOreq: check for collission with properties and signals/slots too, which are all technically "members" so I may refactor to account for that fact. actually tbh I'm not sure if methods collide, but they might (and I usually assume they DO to be on the safe side). it also applies to getters/setters/changedNotifiers
{
    int indexCurrentlyTestingForNameCollission = -1;
    QString ret;
    do
    {
        ret = "m_" + DesignEqualsImplementationProjectGenerator::firstCharacterToUpper(childMemberClassType->Name) + QString::number(++indexCurrentlyTestingForNameCollission); //m_Foo0, m_Foo1, etc. TODOoptional: random 5 letter word from dictionary chosen, append two numbers also, so they are easier to differentiate/remember when using auto mode (although i probably won't use it myself (unless i'm in a rush)). //TODooptional: should the first m_Foo have a zero on the end or no? I'd say yes keep the zero, just makes it simpler LATER
    }
    while(memberWithNameExists(ret));
    return ret;
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
    //TODOmb: analyze Qualifiers_RHS for an asterisk??? if so, should probably do it during parseQualifiedTypeString and then set the enum accordingly (default to non-owner pointer when asterisk seen, ofc). additionally, i should first check that it's not ALREADY set to OwnsPointedToData. if that's the case, just 'return;' without any asterisk analysis
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
