#include "designequalsimplementationclassprivatemethod.h"

#include <QDataStream>

#define DesignEqualsImplementationClassPrivateMethod_QDS(qds, direction, designEqualsImplementationClassPrivateMethod) \
qds direction designEqualsImplementationClassPrivateMethod.Name; \
qds direction designEqualsImplementationClassPrivateMethod.ReturnType; \
qds direction designEqualsImplementationClassPrivateMethod.Arguments; \
return qds;

DesignEqualsImplementationClassPrivateMethod::DesignEqualsImplementationClassPrivateMethod(QObject *parent)
    : QObject(parent)
    , IDesignEqualsImplementationHaveOrderedListOfStatements()
{ }
DesignEqualsImplementationClassPrivateMethod::~DesignEqualsImplementationClassPrivateMethod()
{
    qDeleteAll(Arguments);
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassPrivateMethod &designEqualsImplementationClassPrivateMethod)
{
    DesignEqualsImplementationClassPrivateMethod_QDS(out, <<, designEqualsImplementationClassPrivateMethod);
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassPrivateMethod &designEqualsImplementationClassPrivateMethod)
{
    DesignEqualsImplementationClassPrivateMethod_QDS(in, >>, designEqualsImplementationClassPrivateMethod);
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassPrivateMethod *&designEqualsImplementationClassPrivateMethod)
{
    return out << *designEqualsImplementationClassPrivateMethod;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassPrivateMethod *&designEqualsImplementationClassPrivateMethod)
{
    designEqualsImplementationClassPrivateMethod = new DesignEqualsImplementationClassPrivateMethod();
    return in >> *designEqualsImplementationClassPrivateMethod;
}
