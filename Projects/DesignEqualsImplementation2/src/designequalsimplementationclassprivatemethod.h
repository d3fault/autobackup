#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSPRIVATEMETHOD_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSPRIVATEMETHOD_H

#include "idesignequalsimplementationhaveorderedlistofstatements.h"

#include <QObject>
#include <QList>

#include "idesignequalsimplementationmethod.h"
#include "designequalsimplementationclassmethodargument.h"

class DesignEqualsImplementationClassPrivateMethod : public QObject, public IDesignEqualsImplementationHaveOrderedListOfStatements, public IDesignEqualsImplementationMethod
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassPrivateMethod(QObject *parent = 0);

    QString ReturnType;

    virtual QObject *asQObject();

    static void streamOutPrivateMethodReference(DesignEqualsImplementationProject *project, DesignEqualsImplementationClassPrivateMethod *privateMethod, QDataStream &out);
    static DesignEqualsImplementationClassPrivateMethod* streamInPrivateMethodReference(DesignEqualsImplementationProject *project, QDataStream &in);    
signals:
    void statementInserted(int indexInsertedInto, IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements *statementInserted);
};
#if 0
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassPrivateMethod &designEqualsImplementationClassPrivateMethod);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassPrivateMethod &designEqualsImplementationClassPrivateMethod);
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassPrivateMethod *designEqualsImplementationClassPrivateMethod);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassPrivateMethod *designEqualsImplementationClassPrivateMethod);
#endif

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSPRIVATEMETHOD_H
