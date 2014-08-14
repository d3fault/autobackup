#include "designequalsimplementationclassproperty.h"

#include <QDataStream>

#define DesignEqualsImplementationClassProperty_QDS(qds, direction, designEqualsImplementationClassProperty) \
qds direction designEqualsImplementationClassProperty.Type; \
qds direction designEqualsImplementationClassProperty.Name; \
qds direction designEqualsImplementationClassProperty.ReadOnly; \
qds direction designEqualsImplementationClassProperty.NotifiesOnChange; \
return qds;

DesignEqualsImplementationClassProperty::DesignEqualsImplementationClassProperty(QObject *parent)
    : QObject(parent)
    , ReadOnly(false)
    , NotifiesOnChange(false)
{ }
DesignEqualsImplementationClassProperty::DesignEqualsImplementationClassProperty(const QString &propertyType, const QString &propertyName, bool readOnly, bool notifiesOnChange, QObject *parent)
    : QObject(parent)
    , Type(propertyType)
    , Name(propertyName)
    , ReadOnly(readOnly)
    , NotifiesOnChange(notifiesOnChange)
{ }
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassProperty &designEqualsImplementationClassProperty)
{
    DesignEqualsImplementationClassProperty_QDS(out, <<, designEqualsImplementationClassProperty);
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassProperty &designEqualsImplementationClassProperty)
{
    DesignEqualsImplementationClassProperty_QDS(in, >>, designEqualsImplementationClassProperty);
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassProperty *&designEqualsImplementationClassProperty)
{
    return out << *designEqualsImplementationClassProperty;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassProperty *&designEqualsImplementationClassProperty)
{
    designEqualsImplementationClassProperty = new DesignEqualsImplementationClassProperty();
    return in >> *designEqualsImplementationClassProperty;
}
