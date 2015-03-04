#include "idesignequalsimplementationhaveorderedlistofstatements.h"

void IDesignEqualsImplementationHaveOrderedListOfStatements::insertStatementIntoOrderedListOfStatements(int indexToInsertInto_WhereZeroComesBeforeTheFirstEntryAndOneMeansAfterTheFirstEntryAndSoOn, IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements *statementToInsert)
{
    m_OrderedListOfStatements.insert(indexToInsertInto_WhereZeroComesBeforeTheFirstEntryAndOneMeansAfterTheFirstEntryAndSoOn, statementToInsert); //TODOreq: i'm pretty sure the index passed to me was in "index into m_VerticalSnapPositions" and needs to be converted to this ordered list of statements index. i think i'm getting lucky with low values and the bug will only be shown on the 2nd+ index. it also has to be shown in the sense of the gui, which currently isn't, so maybe i'm already being affected by it? this also applies to existing signal statement's index thingo, so both need to be accounted for

    //emit not declared at this scope :(. good thing it works anyways :)
    /*emit */statementInserted(indexToInsertInto_WhereZeroComesBeforeTheFirstEntryAndOneMeansAfterTheFirstEntryAndSoOn, statementToInsert);
}
QList<IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements *> IDesignEqualsImplementationHaveOrderedListOfStatements::orderedListOfStatements() const
{
    return m_OrderedListOfStatements;
}
