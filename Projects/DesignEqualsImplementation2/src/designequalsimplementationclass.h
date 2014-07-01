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
    QList<DesignEqualsImplementationClass*> PrivateMembers; //TODOreq: POD
    QList<DesignEqualsImplementationClassPrivateMethod*> PrivateMethods;
    QList<DesignEqualsImplementationClassSlot*> Slots;
    QList<DesignEqualsImplementationClassSignal*> Signals;

    QList<QString> allMyAvailableMemberGettersWhenInAnyOfMyOwnSlots_AsString();
signals:
    void e(const QString &);
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClass &designEqualsImplementationClass);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClass &designEqualsImplementationClass);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClass *&designEqualsImplementationClass);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClass *&designEqualsImplementationClass);

#endif // DESIGNEQUALSIMPLEMENTATIONCLASS_H
