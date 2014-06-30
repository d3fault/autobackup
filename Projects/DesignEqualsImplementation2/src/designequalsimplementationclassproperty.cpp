#include "designequalsimplementationclassproperty.h"

#include <QDataStream>

#define DesignEqualsImplementationClassProperty_QDS(qds, direction, designEqualsImplementationClassProperty) \
qds direction designEqualsImplementationClassProperty.Name; \
qds direction designEqualsImplementationClassProperty.Type; \
qds direction designEqualsImplementationClassProperty.ReadOnly; \
qds direction designEqualsImplementationClassProperty.NotifiesOnChange; \
return qds;

DesignEqualsImplementationClassProperty::DesignEqualsImplementationClassProperty(QObject *parent)
    : QObject(parent)
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
