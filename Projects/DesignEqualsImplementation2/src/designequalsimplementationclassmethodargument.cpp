#include "designequalsimplementationclassmethodargument.h"

#include <QDataStream>

#define DesignEqualsImplementationClassMethodArgument_QDS(qds, direction, argument) \
qds direction argument.Type; \
qds direction argument.VariableName; \
return qds;

DesignEqualsImplementationClassMethodArgument::DesignEqualsImplementationClassMethodArgument(QObject *parent)
    : QObject(parent)
{ }
DesignEqualsImplementationClassMethodArgument::DesignEqualsImplementationClassMethodArgument(const QString &variableName, QObject *parent)
    : QObject(parent)
    , IHaveTypeAndVariableNameAndPreferredTextualRepresentation(variableName)
{ }
DesignEqualsImplementationClassMethodArgument::~DesignEqualsImplementationClassMethodArgument()
{ }
QString DesignEqualsImplementationClassMethodArgument::typeString()
{
    return Type;
}
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassMethodArgument &argument)
{
    DesignEqualsImplementationClassMethodArgument_QDS(out, <<, argument)
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassMethodArgument &argument)
{
    DesignEqualsImplementationClassMethodArgument_QDS(in, >>, argument)
}
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassMethodArgument *argument)
{
    out << *argument;
    return out;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassMethodArgument *argument)
{
    argument = new DesignEqualsImplementationClassMethodArgument();
    in >> *argument;
    return in;
}
