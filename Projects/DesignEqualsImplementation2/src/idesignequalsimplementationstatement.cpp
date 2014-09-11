#include "idesignequalsimplementationstatement.h"

#include <QDataStream>

#include "designequalsimplementationsignalemissionstatement.h"
#include "designequalsimplementationslotinvocationstatement.h"
#include "designequalsimplementationprivatemethodsynchronouscallstatement.h"

#if 0
QDataStream &operator<<(QDataStream &out, IDesignEqualsImplementationStatement *&statement)
{
    out << static_cast<quint8>(statement->StatementType);
    statement->streamOut(out);
    return out;
    //return out << *statement;
}
QDataStream &operator>>(QDataStream &in, IDesignEqualsImplementationStatement *&statement)
{
    quint8 statementType;
    in >> statementType;
    statement->StatementType = static_cast<IDesignEqualsImplementationStatement::StatementTypeEnum>(statementType);
    switch(statement->StatementType)
    {
    case IDesignEqualsImplementationStatement::SignalEmitStatementType:
        statement = new DesignEqualsImplementationSignalEmissionStatement();
        break;
    case IDesignEqualsImplementationStatement::SlotInvokeStatementType:
        statement = new DesignEqualsImplementationSlotInvocationStatement();
        break;
    case IDesignEqualsImplementationStatement::PrivateMethodSynchronousCallStatementType:
        statement = new DesignEqualsImplementationPrivateMethodSynchronousCallStatement();
        break;
    }
    statement->streamIn(in);
    return in;
    //return in >> *statement;
}
#endif
