#include "designequalsimplementationclasssignal.h"

#include <QDataStream>

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
