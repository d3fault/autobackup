#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSSLOT_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSSLOT_H

#include <QObject>
#include <QList>

#include "designequalsimplementationclassmethodargument.h"

class DesignEqualsImplementationClassSlot : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassSlot(QObject *parent = 0);
    ~DesignEqualsImplementationClassSlot();

    //TODOoptional: private + getter/setter blah
    QString Name;
    QList<DesignEqualsImplementationClassMethodArgument*> Arguments;

    //TODOoptional: return type
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassSlot &slot);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSlot &slot);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassSlot *&slot);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSlot *&slot);

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSSLOT_H
