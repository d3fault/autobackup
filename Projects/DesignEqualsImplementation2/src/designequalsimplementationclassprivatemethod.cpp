#include "designequalsimplementationclassprivatemethod.h"

#include <QDataStream>

#define DesignEqualsImplementationClassPrivateMethod_QDS(qds, direction, designEqualsImplementationClassPrivateMethod) \
qds direction designEqualsImplementationClassPrivateMethod.Name; \
qds direction designEqualsImplementationClassPrivateMethod.ReturnType; \
qds direction designEqualsImplementationClassPrivateMethod.Arguments; \
return qds; //TODOoptimization: argument types should be serialized using a dictionary of types. it should only be used during [de-]serialization because of the difference between type rename and change-type-of-existing-arg -- and the confusion that trying to maintain a dictionary would add on top. compression alone might alleviate the dupe type specifying overhead ENOUGH

DesignEqualsImplementationClassPrivateMethod::DesignEqualsImplementationClassPrivateMethod(QObject *parent)
    : QObject(parent)
    , IDesignEqualsImplementationHaveOrderedListOfStatements()
{ }
DesignEqualsImplementationClassPrivateMethod::~DesignEqualsImplementationClassPrivateMethod()
{
    qDeleteAll(Arguments);
}
QObject *DesignEqualsImplementationClassPrivateMethod::asQObject()
{
    return this;
}
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassPrivateMethod &designEqualsImplementationClassPrivateMethod)
{
    DesignEqualsImplementationClassPrivateMethod_QDS(out, <<, designEqualsImplementationClassPrivateMethod);
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassPrivateMethod &designEqualsImplementationClassPrivateMethod)
{
    DesignEqualsImplementationClassPrivateMethod_QDS(in, >>, designEqualsImplementationClassPrivateMethod);
}
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassPrivateMethod *designEqualsImplementationClassPrivateMethod)
{
    out << *designEqualsImplementationClassPrivateMethod;
    return out;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassPrivateMethod *designEqualsImplementationClassPrivateMethod)
{
    designEqualsImplementationClassPrivateMethod = new DesignEqualsImplementationClassPrivateMethod();
    in >> *designEqualsImplementationClassPrivateMethod;
    return in;
}
