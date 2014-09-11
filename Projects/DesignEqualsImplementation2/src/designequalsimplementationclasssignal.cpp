#include "designequalsimplementationclasssignal.h"

#include <QDataStream>

#include "designequalsimplementationproject.h"

#define DesignEqualsImplementationClassSignal_QDS(qds, direction, signal) \
qds direction signal.Name; \
qds direction signal.m_Arguments; \
return qds;

DesignEqualsImplementationClassSignal::DesignEqualsImplementationClassSignal(QObject *parent)
    : QObject(parent)
    , IDesignEqualsImplementationMethod()
{ }
DesignEqualsImplementationClassSignal::~DesignEqualsImplementationClassSignal()
{ }
DesignEqualsImplementationClassSignal *DesignEqualsImplementationClassSignal::streamInSignalReference(DesignEqualsImplementationProject *project, QDataStream &in)
{
    int classIdOfClassThatHasSignal;
    in >> classIdOfClassThatHasSignal;
    int signalId;
    in >> signalId;
    DesignEqualsImplementationClass *classThatHasSignal = project->classInstantiationFromSerializedClassId(classIdOfClassThatHasSignal);
    return classThatHasSignal->signalInstantiationFromSerializedSignalId(signalId);
}
void DesignEqualsImplementationClassSignal::streamOutSignalReference(DesignEqualsImplementationProject *project, DesignEqualsImplementationClassSignal *signal, QDataStream &out)
{
    out << project->serializationClassIdForClass(signal->ParentClass);
    out << signal->ParentClass->serializationSignalIdForSignal(signal);
}
#if 0
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassSignal &signal)
{
    DesignEqualsImplementationClassSignal_QDS(out, <<, signal)
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSignal &signal)
{
    DesignEqualsImplementationClassSignal_QDS(in, >>, signal)
}
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassSignal *signal)
{
    out << *signal;
    return out;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSignal *signal)
{
    signal = new DesignEqualsImplementationClassSignal();
    in >> *signal;
    return in;
}
#endif
