#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSSIGNAL_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSSIGNAL_H

#include <QObject>
#include <QList>

#include "designequalsimplementationclassmethodargument.h"

class DesignEqualsImplementationClassSignal : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassSignal(QObject *parent = 0);
    ~DesignEqualsImplementationClassSignal();

    //TODOoptional: private + getter/setter blah
    QString Name;
    QList<DesignEqualsImplementationClassMethodArgument*> Arguments;
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassSignal &signal);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSignal &signal);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassSignal *&signal);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSignal *&signal);

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSSIGNAL_H
