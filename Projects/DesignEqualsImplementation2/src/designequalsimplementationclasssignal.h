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

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSSIGNAL_H
