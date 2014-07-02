#include "designequalsimplementationclasssignal.h"

#include <QDataStream>

#define DesignEqualsImplementationClassSignal_QDS(qds, direction, signal) \
qds direction signal.Name; \
qds direction signal.Arguments; \
return qds;

DesignEqualsImplementationClassSignal::DesignEqualsImplementationClassSignal(QObject *parent)
    : QObject(parent)
    , IDesignEqualsImplementationMethod()
{ }
DesignEqualsImplementationClassSignal::~DesignEqualsImplementationClassSignal()
{ }
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassSignal &signal)
{
    DesignEqualsImplementationClassSignal_QDS(out, <<, signal)
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSignal &signal)
{
    DesignEqualsImplementationClassSignal_QDS(in, >>, signal)
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassSignal *&signal)
{
    return out << *signal;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSignal *&signal)
{
    signal = new DesignEqualsImplementationClassSignal();
    return in >> *signal;
}
