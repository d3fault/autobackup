#ifndef IDESIGNEQUALSIMPLEMENTATIONSTATEMENT_H
#define IDESIGNEQUALSIMPLEMENTATIONSTATEMENT_H

#include <QString>

class IDesignEqualsImplementationStatement
{
public:
    enum StatementTypeEnum
    {
          SignalEmitStatementType
        , SlotInvokeStatementType
        , PrivateMethodSynchronousCallStatementType
    };
    explicit IDesignEqualsImplementationStatement(StatementTypeEnum statementType) : StatementType(statementType) { }
    virtual ~IDesignEqualsImplementationStatement() { }
    virtual QString toRawCppWithEndingSemicolon() { return toRawCppWithoutEndingSemicolon() + ";"; }
    virtual QString toRawCppWithoutEndingSemicolon()=0; //if/else/while/etc can't have it
    virtual bool isSignalEmit() { return false; }
    virtual bool isSlotInvoke() { return false; }

    virtual void streamIn(QDataStream &in)=0;
    virtual void streamOut(QDataStream &out)=0;
    StatementTypeEnum StatementType; //only needed for [de]-serialization methinks
};

QDataStream &operator<<(QDataStream &out, const IDesignEqualsImplementationStatement *&statement);
QDataStream &operator>>(QDataStream &in, IDesignEqualsImplementationStatement *&statement);

//Q_DECLARE_METATYPE(IDesignEqualsImplementationStatement*)

#endif // IDESIGNEQUALSIMPLEMENTATIONSTATEMENT_H
