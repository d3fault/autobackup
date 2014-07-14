#ifndef IDESIGNEQUALSIMPLEMENTATIONHAVEORDEREDLISTOFSTATEMENTS_H
#define IDESIGNEQUALSIMPLEMENTATIONHAVEORDEREDLISTOFSTATEMENTS_H

#include <QList>
#include <idesignequalsimplementationstatement.h>

class QObject;

class IDesignEqualsImplementationHaveOrderedListOfStatements
{
public:
    explicit IDesignEqualsImplementationHaveOrderedListOfStatements() { }
    virtual ~IDesignEqualsImplementationHaveOrderedListOfStatements() { qDeleteAll(m_OrderedListOfStatements); }

    //TODOreq: getter/setter (signal emit)
    void insertStatementIntoOrderedListOfStatements(int indexToInsertInto_WhereZeroComesBeforeTheFirstEntryAndOneMeansAfterTheFirstEntryAndSoOn, IDesignEqualsImplementationStatement *statementToInsert);
    QList<IDesignEqualsImplementationStatement*> orderedListOfStatements() const;
    virtual QObject *asQObject()=0;
private:
    QList<IDesignEqualsImplementationStatement*> m_OrderedListOfStatements;

public: //signals
    virtual void statementInserted(int indexInsertedInto, IDesignEqualsImplementationStatement *statementInserted)=0;
};

#endif // IDESIGNEQUALSIMPLEMENTATIONHAVEORDEREDLISTOFSTATEMENTS_H
