#ifndef IDESIGNEQUALSIMPLEMENTATIONSTATEMENT_H
#define IDESIGNEQUALSIMPLEMENTATIONSTATEMENT_H

#include <QString>

class DesignEqualsImplementationProject;

class IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements
{
public:
    enum StatementTypeEnum
    {
          SignalEmitStatementType
        , SlotInvokeStatementType
        , PrivateMethodSynchronousCallStatementType
        , ChunkOfRawCppStatements
    };
    explicit IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements(StatementTypeEnum statementType) : StatementType(statementType) { }
    virtual ~IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements() { }
    QString toRawCpp();
    QString toRawCppWithEndingSemicolon() { return toRawCppWithoutEndingSemicolon() + ";"; }
    virtual QString toRawCppWithoutEndingSemicolon()=0; //if/else/while/etc can't have it
    virtual bool isSignalEmit() { return false; }
    virtual bool isSlotInvoke() { return false; }

    virtual void streamIn(DesignEqualsImplementationProject *project, QDataStream &in)=0;
    virtual void streamOut(DesignEqualsImplementationProject *project, QDataStream &out)=0;
    StatementTypeEnum StatementType; //only needed for [de]-serialization methinks
};

#if 0
QDataStream &operator<<(QDataStream &out, const IDesignEqualsImplementationStatement *&statement);
QDataStream &operator>>(QDataStream &in, IDesignEqualsImplementationStatement *&statement);
#endif

//Q_DECLARE_METATYPE(IDesignEqualsImplementationStatement*)

#endif // IDESIGNEQUALSIMPLEMENTATIONSTATEMENT_H
