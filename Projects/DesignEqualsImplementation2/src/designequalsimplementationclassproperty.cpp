#include "designequalsimplementationclassproperty.h"

#include <QDataStream>

#include "designequalsimplementationprojectgenerator.h"

//TODOreq: WRT thread-safety, should the "write" be a slot? Should the read even exist, since it isn't thread safe? The notification signal is thread-safe however. The read method could be made thread-safe by calling it "readValueRequested", and it would have to be responded to through a signal. I'm not sure any of this is worth it. Also, changing the 'read' to be thread-safe like that would make it incompatible with the meta-object system, thus defeating purpose of making it Q_PROPERTY to begin with. Still, the overall concept of "thread safe property getting and setting" does make a ton of sense to me.
#if 0
DesignEqualsImplementationClassProperty::DesignEqualsImplementationClassProperty(QObject *parent)
    : NonFunctionMember(parent)
    , ReadOnly(false)
    , NotifiesOnChange(false)
{ }
#endif
DesignEqualsImplementationClassProperty::DesignEqualsImplementationClassProperty(Type *propertyType, const QString &propertyName, Type *parentClassThatIamPropertyOf, QObject *parent, bool hasInit, const QString &optionalInit, bool readOnly, bool notifiesOnChange)
    : NonFunctionMember(propertyType, DesignEqualsImplementationProjectGenerator::memberNameForProperty(propertyName), parentClassThatIamPropertyOf, parent, hasInit, optionalInit) //hack using generator like this.... but then again this entire fucking app is a hack. so ugly. I can't wait to redesign it in itself
    //, PropertyType(propertyType)
    , PropertyName(propertyName)
    , ReadOnly(readOnly)
    , NotifiesOnChange(notifiesOnChange)
{ }
