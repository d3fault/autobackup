#include "designequalsimplementationclassmethodargument.h"

#include <QDataStream>

#define DesignEqualsImplementationClassMethodArgument_QDS(qds, direction, argument) \
qds direction argument.Type; \
qds direction argument.Name; \
return qds;

DesignEqualsImplementationClassMethodArgument::DesignEqualsImplementationClassMethodArgument(QObject *parent)
    : QObject(parent)
{ }
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassMethodArgument &argument)
{
    DesignEqualsImplementationClassMethodArgument_QDS(out, <<, argument)
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassMethodArgument &argument)
{
    DesignEqualsImplementationClassMethodArgument_QDS(in, >>, argument)
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassMethodArgument *&argument)
{
    return out << *argument;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassMethodArgument *&argument)
{
    argument = new DesignEqualsImplementationClassMethodArgument();
    return in >> *argument;
}
