#include "designequalsimplementationclassprivatemethod.h"

#include <QDataStream>

#include "designequalsimplementationproject.h"

#define DesignEqualsImplementationClassPrivateMethod_QDS(qds, direction, designEqualsImplementationClassPrivateMethod) \
qds direction designEqualsImplementationClassPrivateMethod.Name; \
qds direction designEqualsImplementationClassPrivateMethod.ReturnType; \
qds direction designEqualsImplementationClassPrivateMethod.Arguments; \
return qds; //TODOoptimization: argument types should be serialized using a dictionary of types. it should only be used during [de-]serialization because of the difference between type rename and change-type-of-existing-arg -- and the confusion that trying to maintain a dictionary would add on top. compression alone might alleviate the dupe type specifying overhead ENOUGH

//TODOreq: this->ParentClass needs to be set during DesignEqualsImplementationClass::addPrivateMethod() -- just like in addSlot and addSignal. but I don't have the code even using this class yet so...
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
    out << project->serializationTypeIdForType(privateMethod->ParentClass);
    out << privateMethod->ParentClass->serializationPrivateMethodIdForPrivateMethod(privateMethod);
}
DesignEqualsImplementationClassPrivateMethod *DesignEqualsImplementationClassPrivateMethod::streamInPrivateMethodReference(DesignEqualsImplementationProject *project, QDataStream &in)
{
    int classIdOfClassThatHasPrivateMethod;
    in >> classIdOfClassThatHasPrivateMethod;
    int privateMethodId;
    in >> privateMethodId;
    DesignEqualsImplementationClass *classThatHasPrivateMethod = qobject_cast<DesignEqualsImplementationClass*>(project->typeFromSerializedTypeId(classIdOfClassThatHasPrivateMethod));
    if(!classThatHasPrivateMethod)
        qFatal("While trying to deserialize a private method, the type for which the private method allegedly belongs failed to be cast into a DesignEqualsImplementationClass. This is likely a programming bug on the serialization side of things, but who knows");
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
