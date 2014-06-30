#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSPRIVATEMETHOD_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSPRIVATEMETHOD_H

#include <QObject>
#include <QList>

#include "designequalsimplementationclassmethodargument.h"

class DesignEqualsImplementationClassPrivateMethod : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassPrivateMethod(QObject *parent = 0);
    ~DesignEqualsImplementationClassPrivateMethod();

    //TODOoptional: private + getter/setter blah
    QString Name;
    QString ReturnType;
    QList<DesignEqualsImplementationClassMethodArgument*> Arguments;
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassPrivateMethod &designEqualsImplementationClassPrivateMethod);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassPrivateMethod &designEqualsImplementationClassPrivateMethod);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassPrivateMethod *&designEqualsImplementationClassPrivateMethod);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassPrivateMethod *&designEqualsImplementationClassPrivateMethod);

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSPRIVATEMETHOD_H
