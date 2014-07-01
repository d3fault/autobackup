#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSSLOT_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSSLOT_H

#include "idesignequalsimplementationhaveorderedlistofstatements.h"

#include <QObject>
#include <QList>

#include "designequalsimplementationclassmethodargument.h"

class DesignEqualsImplementationClass;

class DesignEqualsImplementationClassSlot : public QObject, public IDesignEqualsImplementationHaveOrderedListOfStatements
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassSlot(QObject *parent = 0);
    virtual ~DesignEqualsImplementationClassSlot();

    //TODOoptional: private + getter/setter blah
    QString Name;
    QList<DesignEqualsImplementationClassMethodArgument*> Arguments;

    //TODOoptional: return type

    DesignEqualsImplementationClass *ParentClass; //TODOreq: [de-]serialization
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassSlot &slot);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSlot &slot);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassSlot *&slot);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSlot *&slot);

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSSLOT_H
