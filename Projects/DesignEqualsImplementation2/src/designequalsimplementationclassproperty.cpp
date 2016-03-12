#include "designequalsimplementationclassproperty.h"

#include <QDataStream>

#include "designequalsimplementationprojectgenerator.h"

#define DesignEqualsImplementationClassProperty_QDS(qds, direction, designEqualsImplementationClassProperty) \
qds direction designEqualsImplementationClassProperty.Type; \
qds direction designEqualsImplementationClassProperty.Name; \
qds direction designEqualsImplementationClassProperty.HasInit; \
if(designEqualsImplementationClassProperty.HasInit) \
qds direction designEqualsImplementationClassProperty.OptionalInit; \
qds direction designEqualsImplementationClassProperty.ReadOnly; \
qds direction designEqualsImplementationClassProperty.NotifiesOnChange; \
return qds;

//TODOreq: WRT thread-safety, should the "write" be a slot? Should the read even exist, since it isn't thread safe? The notification signal is thread-safe however. The read method could be made thread-safe by calling it "readValueRequested", and it would have to be responded to through a signal. I'm not sure any of this is worth it. Also, changing the 'read' to be thread-safe like that would make it incompatible with the meta-object system, thus defeating purpose of making it Q_PROPERTY to begin with. Still, the overall concept of "thread safe property getting and setting" does make a ton of sense to me.
DesignEqualsImplementationClassProperty::DesignEqualsImplementationClassProperty(QObject *parent)
    : TypeInstance(parent)
    , IHaveTypeAndVariableNameAndPreferredTextualRepresentation()
    , HasInit(false)
    , ReadOnly(false)
    , NotifiesOnChange(false)
{ }
DesignEqualsImplementationClassProperty::DesignEqualsImplementationClassProperty(const QString &propertyType, const QString &propertyName, bool hasInit, const QString &optionalInit, bool readOnly, bool notifiesOnChange, QObject *parent)
    : TypeInstance(parent)
    , IHaveTypeAndVariableNameAndPreferredTextualRepresentation(DesignEqualsImplementationProjectGenerator::memberNameForProperty(propertyName)) //hack using generator like this.... but then again this entire fucking app is a hack. so ugly. I can't wait to redesign it in itself
    , Type(propertyType)
    , Name(propertyName)
    , HasInit(hasInit)
    , OptionalInit(optionalInit)
    , ReadOnly(readOnly)
    , NotifiesOnChange(notifiesOnChange)
{ }
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassProperty &designEqualsImplementationClassProperty)
{
    DesignEqualsImplementationClassProperty_QDS(out, <<, designEqualsImplementationClassProperty);
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassProperty &designEqualsImplementationClassProperty)
{
    DesignEqualsImplementationClassProperty_QDS(in, >>, designEqualsImplementationClassProperty);
}
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassProperty *designEqualsImplementationClassProperty)
{
    out << *designEqualsImplementationClassProperty;
    return out;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassProperty *designEqualsImplementationClassProperty)
{
    designEqualsImplementationClassProperty = new DesignEqualsImplementationClassProperty();
    in >> *designEqualsImplementationClassProperty;
    return in;
}
