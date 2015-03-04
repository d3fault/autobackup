#ifndef IDESIGNEQUALSIMPLEMENTATIONHAVEORDEREDLISTOFSTATEMENTS_H
#define IDESIGNEQUALSIMPLEMENTATIONHAVEORDEREDLISTOFSTATEMENTS_H

#include <QList>

#include "idesignequalsimplementationstatement.h"

class QObject;

class IDesignEqualsImplementationHaveOrderedListOfStatements
{
public:
    explicit IDesignEqualsImplementationHaveOrderedListOfStatements() { }
    virtual ~IDesignEqualsImplementationHaveOrderedListOfStatements() { /*TODOreq: ordered list of statements is copied from slot to slot, so need different memory management strategy qDeleteAll(m_OrderedListOfStatements);*/ }

    //TODOreq: getter/setter (signal emit)
    void insertStatementIntoOrderedListOfStatements(int indexToInsertInto_WhereZeroComesBeforeTheFirstEntryAndOneMeansAfterTheFirstEntryAndSoOn, IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements *statementToInsert);
    QList<IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements*> orderedListOfStatements() const;
    virtual QObject *asQObject()=0;

    //I think actually since I use these members in the stream IN (loading, aka writing to the variables) sense, that a const getter is entirely inappropriate and the compiler knew that :). Still seems ugly for them to be public, but oh well, functionality >
    QList<IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements*> m_OrderedListOfStatements;
public: //signals
    virtual void statementInserted(int indexInsertedInto, IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements *statementInserted)=0;
};

#endif // IDESIGNEQUALSIMPLEMENTATIONHAVEORDEREDLISTOFSTATEMENTS_H
