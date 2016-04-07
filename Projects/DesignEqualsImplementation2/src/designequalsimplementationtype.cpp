#include "designequalsimplementationtype.h"

#include <QStringList>

#include "designequalsimplementationprojectgenerator.h"
#include "designequalsimplementationclass.h"
#include "designequalsimplementationclassproperty.h"
#include "designequalsimplementationclassmethodargument.h"

//tl;dr: using C++ to model C++. My brain is full of fuck and oh it feels so good [man]
NonFunctionMember *DesignEqualsImplementationType::createNewNonFunctionMember(DesignEqualsImplementationType *typeOfNewNonFunctionMember, const QString &qualifiedTypeString, const QString &nameOfNewNonFunctionMember_OrEmptyStringToAutoGenerateOne, Visibility::VisibilityEnum visibility, TypeInstanceOwnershipOfPointedToDataIfPointer::TypeInstanceOwnershipOfPointedToDataIfPointerEnum ownershipOfPointedToDataIfPointer, bool hasInit, const QString &optionalInit)
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
bool DesignEqualsImplementationType::memberWithNameExists(const QString &memberNameToCheckForCollisions) const
{
    //TO DOnemb: for improved readibility (sanity), perhaps I should do case insensitive matching (.toLower both sides). TODOmb: 'lookalike' characters (zero and oh, etc), but however in those fuzzy matches, it should only issue a warning and give user an opportunity to change [or accept as is]
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
    if(const DesignEqualsImplementationClass *typeAsClass = qobject_cast<const DesignEqualsImplementationClass*>(this))
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
QString DesignEqualsImplementationType::autoNameForNewChildMemberOfType(const QString &childMemberClassType) const //TODOreq: check for collission with properties and signals/slots too, which are all technically "members" so I may refactor to account for that fact. actually tbh I'm not sure if methods collide, but they might (and I usually assume they DO to be on the safe side). it also applies to getters/setters/changedNotifiers
{
    int indexCurrentlyTestingForNameCollission = -1;
    QString ret;
    do
    {
        ret = "m_" + DesignEqualsImplementationProjectGenerator::firstCharacterToUpper(childMemberClassType) + QString::number(++indexCurrentlyTestingForNameCollission); //m_Foo0, m_Foo1, etc. TODOoptional: random 5 letter word from dictionary chosen, append two numbers also, so they are easier to differentiate/remember when using auto mode (although i probably won't use it myself (unless i'm in a rush)). //TODooptional: should the first m_Foo have a zero on the end or no? I'd say yes keep the zero, just makes it simpler LATER
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
bool TypeInstance::isPointer(const QString &qualifiers_RHS_or_BothSidesIsFineToo)
{
    //TODOreq: PRETTY SURE you can't have an asterisk on the LHS for a type instance declaration, but if you can then I need to mandate that it's RHS only (and can split by type name to get it if need be)
    return qualifiers_RHS_or_BothSidesIsFineToo.contains("*");
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

    if(isPointer(Qualifiers_RHS) && OwnershipOfPointedTodataIfPointer != TypeInstanceOwnershipOfPointedToDataIfPointer::OwnsPointedToData)
        OwnershipOfPointedTodataIfPointer = TypeInstanceOwnershipOfPointedToDataIfPointer::DoesNotOwnPointedToData; //we see the asterisk and bump up the enum to "does not own pointer". NOTE: it's CRUCIAL that we don't have the ability to set OwnershipOfPointedTodataIfPointer via a constructor arg and it's set later after the constructor, because otherwise this might override ... nvm I can put a safety meausre in :)
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
}
void TypeInstance::streamOutReference(QDataStream &out, DesignEqualsImplementationProject *project)
{
    out << static_cast<quint8>(typeInstanceCategory());

    DesignEqualsImplementationType *typeInstanceOwnerType;
    switch(typeInstanceCategory())
    {
    case SlotArgumentTypeInstance:
    case PrivateMethodArgumentTypeInstance:
        typeInstanceOwnerType = qobject_cast<DesignEqualsImplementationClassMethodArgument*>(this)->ParentMethod->ParentClass;
    break;
    case NonFunctionMemberTypeInstance:
        typeInstanceOwnerType = qobject_cast<NonFunctionMember*>(this)->parentClass();
    break;
    }
    out << project->serializationTypeIdForType(typeInstanceOwnerType);

    if(NonFunctionMember *thisAsNonFunctionMember = qobject_cast<NonFunctionMember*>(this))
    {
        out << thisAsNonFunctionMember->parentClass()->serializationNonFunctionMemberIdForNonFunctionMember(thisAsNonFunctionMember);
        return;
    }

    DesignEqualsImplementationClassMethodArgument *thisAsMethodArgument = qobject_cast<DesignEqualsImplementationClassMethodArgument*>(this);
    if(!thisAsMethodArgument)
        qFatal("TypeInstanceCategory was argument, but failed to cast typeinstance into argument O_o");
    DesignEqualsImplementationClass *typeAsClass = qobject_cast<DesignEqualsImplementationClass*>(typeInstanceOwnerType);
    if(!typeAsClass)
        qFatal("TypeInstanceCategory was argument, but failed to cast Type to Class");

    IDesignEqualsImplementationMethod *method = thisAsMethodArgument->ParentMethod;
    switch(typeInstanceCategory())
    {
    case SlotArgumentTypeInstance:
        out << typeAsClass->serializationSlotIdForSlot(qobject_cast<DesignEqualsImplementationClassSlot*>(method->asQObject()));
    break;
    case PrivateMethodArgumentTypeInstance:
        out << typeAsClass->serializationPrivateMethodIdForPrivateMethod(qobject_cast<DesignEqualsImplementationClassPrivateMethod*>(method->asQObject()));
    break;
    case NonFunctionMemberTypeInstance:
        //already handled above
    break;
    }
    out << method->serializationArgumentIdForArgument(thisAsMethodArgument);
}
int DesignEqualsImplementationType::serializationNonFunctionMemberIdForNonFunctionMember(NonFunctionMember *nonFunctionMember) const
{
    return m_NonFunctionMembers.indexOf(nonFunctionMember);
}
NonFunctionMember *DesignEqualsImplementationType::nonFunctionMemberFromNonFunctionMemberId(int nonFunctionMemberId) const
{
    return m_NonFunctionMembers.at(nonFunctionMemberId);
}
QString DesignEqualsImplementationType::headerFilenameOnly() const
{
    return Name.toLower() + ".h"; //TODOreq: we don't want int.h, bool.h, etc (same with .cpp). Ideally a type can/does specify N headers it requires. Our DesignEqualsImplementationClass, however, does just use type.name.toLower() ofc...
}
QString DesignEqualsImplementationType::sourceFilenameOnly() const
{
    return Name.toLower() + ".cpp";
}
QStringList DesignEqualsImplementationType::includes() const
{
    if(const DefinedElsewhereType *currentNonFunctionMemberTypeAsDefinedElsewhereType = qobject_cast<const DefinedElsewhereType*>(this))
        return currentNonFunctionMemberTypeAsDefinedElsewhereType->definedElsewhereIncludes();
    return QStringList() << headerFilenameOnly();
}
void DesignEqualsImplementationType::addNonFunctionMemberPrivate(NonFunctionMember *nonFunctionMember)
{
    m_NonFunctionMembers << nonFunctionMember;
    /*emit */nonFunctionMemberAdded(nonFunctionMember);
}
void DesignEqualsImplementationType::setClassName(const QString &newClassName)
{
    Name = newClassName; // s/Class/Type (it's a slot)
}
QList<NonFunctionMember *> DesignEqualsImplementationType::nonFunctionMembers_OrderedCorrectlyAsMuchAsPossibleButWithMembersThatHaveOptionalInitAtTheEnd() const
{
    QList<NonFunctionMember*> ret;
    QList<NonFunctionMember*> haveInit;
    Q_FOREACH(NonFunctionMember *currentNonFunctionMember, nonFunctionMembers())
    {
        if(currentNonFunctionMember->HasInit)
            haveInit << currentNonFunctionMember;
        else
            ret << currentNonFunctionMember;
    }
    ret << haveInit;
    return ret;
}
TypeInstance *TypeInstance::streamInReferenceAndReturnPointerToIt(QDataStream &in, DesignEqualsImplementationProject *project)
{
    quint8 typeInstanceCategoryId;
    in >> typeInstanceCategoryId;
    TypeInstanceCategory typeInstanceCategory = static_cast<TypeInstanceCategory>(typeInstanceCategoryId);

    //all categories have this atm, but for example a Global TypeInstance wouldn't (it's 'parent' would be the project itself), in which case the streaming of it needs to go in a/the switch statement
    int serializedTypeId;
    in >> serializedTypeId;
    DesignEqualsImplementationType *typeInstanceOwnerType = project->typeFromSerializedTypeId(serializedTypeId);

    int typeInstanceMemberId; //could be the id of the type instance itself, in the case of NonFunctionMembers, or could be the id of the slot/private-method, in the case of being an _argument_ of one of those
    in >> typeInstanceMemberId;
    switch(typeInstanceCategory)
    {
    case NonFunctionMemberTypeInstance:
    return typeInstanceOwnerType->nonFunctionMemberFromNonFunctionMemberId(typeInstanceMemberId);
    break;
    case SlotArgumentTypeInstance:
    case PrivateMethodArgumentTypeInstance:
        //do nothing (yet)
    break;
    }

    DesignEqualsImplementationClass *typeAsClass = qobject_cast<DesignEqualsImplementationClass*>(typeInstanceOwnerType);
    if(!typeAsClass)
        qFatal("Slot argument or private method argument context variable found, but type was not QObject derived");
    IDesignEqualsImplementationMethod *method;
    switch(typeInstanceCategory)
    {
    case SlotArgumentTypeInstance:
        method = typeAsClass->slotInstantiationFromSerializedSlotId(typeInstanceMemberId);
    break;
    case PrivateMethodArgumentTypeInstance:
        method = typeAsClass->privateMethodInstantatiationFromSerializedPrivateMethodId(typeInstanceMemberId);
    break;
    case NonFunctionMemberTypeInstance:
        //already handled above
    break;
    }
    int argumentIndex; //or argumentId, smaeshit really
    in >> argumentIndex;
    return method->argumentInstantiationFromSerializedArgumentId(argumentIndex);
}
TypeInstance::TypeInstanceCategory NonFunctionMember::typeInstanceCategory()
{
    return NonFunctionMemberTypeInstance;
}
