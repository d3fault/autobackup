#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSSIGNAL_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSSIGNAL_H

#include "idesignequalsimplementationmethod.h"

#include <QObject>
#include <QList>

#include "designequalsimplementationclassmethodargument.h"

class DesignEqualsImplementationClassSignal : public QObject, public IDesignEqualsImplementationMethod
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassSignal(QObject *parent = 0);
    virtual ~DesignEqualsImplementationClassSignal();
};
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassSignal &signal);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSignal &signal);
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassSignal *signal);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSignal *signal);

Q_DECLARE_METATYPE(DesignEqualsImplementationClassSignal*)

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSSIGNAL_H
