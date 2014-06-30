#ifndef DESIGNEQUALSIMPLEMENTATIONCLASS_H
#define DESIGNEQUALSIMPLEMENTATIONCLASS_H

#include <QObject>

#include "designequalsimplementationclassproperty.h"
#include "designequalsimplementationclassprivatemethod.h"
#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclasssignal.h"

class DesignEqualsImplementationClass : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClass(QObject *parent = 0);
    bool generateSourceCode(const QString &destinationDirectoryPath);
    ~DesignEqualsImplementationClass();

    //TODOoptional: private + getter/setter blah
    QString Name;
    QList<DesignEqualsImplementationClassProperty*> Properties;
    QList<DesignEqualsImplementationClassPrivateMethod*> PrivateMethods;
    QList<DesignEqualsImplementationClassSlot*> Slots;
    QList<DesignEqualsImplementationClassSignal*> Signals;
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASS_H
