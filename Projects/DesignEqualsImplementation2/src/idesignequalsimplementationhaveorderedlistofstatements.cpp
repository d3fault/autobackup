#include "idesignequalsimplementationhaveorderedlistofstatements.h"

void IDesignEqualsImplementationHaveOrderedListOfStatements::insertStatementIntoOrderedListOfStatements(int indexToInsertInto_WhereZeroComesBeforeTheFirstEntryAndOneMeansAfterTheFirstEntryAndSoOn, IDesignEqualsImplementationStatement *statementToInsert)
{
    m_OrderedListOfStatements.insert(indexToInsertInto_WhereZeroComesBeforeTheFirstEntryAndOneMeansAfterTheFirstEntryAndSoOn, statementToInsert);

    //emit not declared at this scope :(. good thing it works anyways :)
    /*emit */statementInserted(indexToInsertInto_WhereZeroComesBeforeTheFirstEntryAndOneMeansAfterTheFirstEntryAndSoOn, statementToInsert);
}
QList<IDesignEqualsImplementationStatement *> IDesignEqualsImplementationHaveOrderedListOfStatements::orderedListOfStatements() const
{
    return m_OrderedListOfStatements;
}
