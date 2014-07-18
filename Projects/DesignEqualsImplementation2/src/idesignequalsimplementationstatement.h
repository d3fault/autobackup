#ifndef IDESIGNEQUALSIMPLEMENTATIONSTATEMENT_H
#define IDESIGNEQUALSIMPLEMENTATIONSTATEMENT_H

#include <QString>

class IDesignEqualsImplementationStatement
{
public:
    explicit IDesignEqualsImplementationStatement() { }
    virtual ~IDesignEqualsImplementationStatement() { }
    virtual QString toRawCppWithEndingSemicolon() { return toRawCppWithoutEndingSemicolon() + ";"; }
    virtual QString toRawCppWithoutEndingSemicolon()=0; //if/else/while/etc can't have it
    virtual bool isSignalEmit() { return false; }
    virtual bool isSlotInvoke() { return false; }
};

#endif // IDESIGNEQUALSIMPLEMENTATIONSTATEMENT_H
