#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSSIGNAL_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSSIGNAL_H

#include "idesignequalsimplementationmethod.h"

#include <QObject>
#include <QList>

#include "designequalsimplementationclassmethodargument.h"

class DesignEqualsImplementationProject;

class DesignEqualsImplementationClassSignal : public QObject, public IDesignEqualsImplementationMethod
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassSignal(QObject *parent = 0);
    virtual ~DesignEqualsImplementationClassSignal();

    static DesignEqualsImplementationClassSignal *streamInSignalReference(DesignEqualsImplementationProject *project, QDataStream &in);
    static void streamOutSignalReference(DesignEqualsImplementationProject *project, DesignEqualsImplementationClassSignal *signal, QDataStream &out);
protected:
    QObject *asQObject() { return this; }
};
#if 0
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassSignal &signal);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSignal &signal);
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassSignal *signal);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSignal *signal);
#endif

Q_DECLARE_METATYPE(DesignEqualsImplementationClassSignal*)

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSSIGNAL_H
