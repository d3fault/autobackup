#include "designequalsimplementationclass.h"

#include <QDataStream>

#define DesignEqualsImplementationClass_QDS(qds, direction, designEqualsImplementationClass) \
qds direction designEqualsImplementationClass.Properties; \
qds direction designEqualsImplementationClass.PrivateMembers; \
qds direction designEqualsImplementationClass.PrivateMethods; \
qds direction designEqualsImplementationClass.Slots; \
qds direction designEqualsImplementationClass.Signals; \
return qds;

DesignEqualsImplementationClass::DesignEqualsImplementationClass(QObject *parent)
    : QObject(parent)
{ }
bool DesignEqualsImplementationClass::generateSourceCode(const QString &destinationDirectoryPath)
{
    //TODOreq
    return true;
}
DesignEqualsImplementationClass::~DesignEqualsImplementationClass()
{
    qDeleteAll(Properties);
    qDeleteAll(PrivateMembers);
    qDeleteAll(PrivateMethods);
    qDeleteAll(Slots);
    qDeleteAll(Signals);
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClass &designEqualsImplementationClass)
{
    DesignEqualsImplementationClass_QDS(out, <<, designEqualsImplementationClass);
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClass &designEqualsImplementationClass)
{
    DesignEqualsImplementationClass_QDS(in, >>, designEqualsImplementationClass);
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClass *&designEqualsImplementationClass)
{
    return out << *designEqualsImplementationClass;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClass *&designEqualsImplementationClass)
{
    designEqualsImplementationClass = new DesignEqualsImplementationClass();
    return in >> *designEqualsImplementationClass;
}
