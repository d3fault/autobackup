#include "designequalsimplementationclassprivatemethod.h"

#include <QDataStream>

#include "designequalsimplementationproject.h"

#define DesignEqualsImplementationClassPrivateMethod_QDS(qds, direction, designEqualsImplementationClassPrivateMethod) \
    qds direction designEqualsImplementationClassPrivateMethod.Name; \
    qds direction designEqualsImplementationClassPrivateMethod.ReturnType; \
    qds direction designEqualsImplementationClassPrivateMethod.Arguments; \
    return qds; //TODOoptimization: argument types should be serialized using a dictionary of types. it should only be used during [de-]serialization because of the difference between type rename and change-type-of-existing-arg -- and the confusion that trying to maintain a dictionary would add on top. compression alone might alleviate the dupe type specifying overhead ENOUGH

DesignEqualsImplementationClassPrivateMethod::DesignEqualsImplementationClassPrivateMethod(QObject *parent)
    : QObject(parent)
    , IDesignEqualsImplementationHaveOrderedListOfStatements()
    , IDesignEqualsImplementationMethod()
{ }
QObject *DesignEqualsImplementationClassPrivateMethod::asQObject()
{
    return this;
}

void DesignEqualsImplementationClassPrivateMethod::streamOutPrivateMethodReference(DesignEqualsImplementationProject *project, DesignEqualsImplementationClassPrivateMethod *privateMethod, QDataStream &out)
{
    out << project->serializationClassIdForClass(privateMethod->ParentClass);
    out << privateMethod->ParentClass->serializationPrivateMethodIdForPrivateMethod(privateMethod);
}
DesignEqualsImplementationClassPrivateMethod *DesignEqualsImplementationClassPrivateMethod::streamInPrivateMethodReference(DesignEqualsImplementationProject *project, QDataStream &in)
{
    int classIdOfClassThatHasPrivateMethod;
    in >> classIdOfClassThatHasPrivateMethod;
    int privateMethodId;
    in >> privateMethodId;
    DesignEqualsImplementationClass *classThatHasPrivateMethod = project->classInstantiationFromSerializedClassId(classIdOfClassThatHasPrivateMethod);
    return classThatHasPrivateMethod->privateMethodInstantatiationFromSerializedPrivateMethodId(privateMethodId);
}
#if 0
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
#endif
