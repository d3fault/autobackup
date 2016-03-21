#include "designequalsimplementationclassproperty.h"

#include <QDataStream>

#include "designequalsimplementationprojectgenerator.h"

//TODOreq: WRT thread-safety, should the "write" be a slot? Should the read even exist, since it isn't thread safe? The notification signal is thread-safe however. The read method could be made thread-safe by calling it "readValueRequested", and it would have to be responded to through a signal. I'm not sure any of this is worth it. Also, changing the 'read' to be thread-safe like that would make it incompatible with the meta-object system, thus defeating purpose of making it Q_PROPERTY to begin with. Still, the overall concept of "thread safe property getting and setting" does make a ton of sense to me.
DesignEqualsImplementationClassProperty::DesignEqualsImplementationClassProperty(Type *propertyType, const QString &qualifiedTypeString, const QString &propertyName, Type *parentClassThatIamPropertyOf, QObject *parent, bool hasInit, const QString &optionalInit, bool readOnly, bool notifiesOnChange)
    : NonFunctionMember(propertyType, qualifiedTypeString, DesignEqualsImplementationProjectGenerator::memberNameForProperty(propertyName), parentClassThatIamPropertyOf, parent, hasInit, optionalInit) //hack using generator like this.... but then again this entire fucking app is a hack. so ugly. I can't wait to redesign it in itself
    , m_PropertyName(propertyName) //TO DOnereq: renaming this should also update TypeInstance::VariableName (since it derives from PropertyName)
    , ReadOnly(readOnly)
    , NotifiesOnChange(notifiesOnChange)
{ }
QString DesignEqualsImplementationClassProperty::propertyName() const
{
    return m_PropertyName;
}
void DesignEqualsImplementationClassProperty::setPropertyName(const QString &propertyName)
{
    //TODOmb: if they type 'm_X' for the propertyName, we could parse/interpret it to mean 'x'. this might be one way of solving the below TODOreq even
    m_PropertyName = propertyName;
    VariableName = DesignEqualsImplementationProjectGenerator::memberNameForProperty(propertyName); //TODOreq: what if some code does NonFunctionMember->VarialbeName = "blah";. virtuals? protected inheritence? qFatal? don't? detect/parse/strip (in a virtual) 'm_' and setFirstCharacterToLower then assign to propertyName (so they both update each other?) <-- but what if no m_ was specified huehuehue (simple. INSERT IT!)
}
