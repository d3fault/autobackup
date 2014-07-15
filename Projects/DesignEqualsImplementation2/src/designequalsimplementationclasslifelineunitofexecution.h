#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTION_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTION_H

#if 0
#include <QObject>

#include "designequalsimplementationusecase.h"

class DesignEqualsImplementationClassLifeLine;

class DesignEqualsImplementationClassLifeLineUnitOfExecution : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassLifeLineUnitOfExecution(DesignEqualsImplementationClassLifeLine *designEqualsImplementationClassLifeLine, QObject *parent = 0);
    DesignEqualsImplementationClassLifeLine *designEqualsImplementationClassLifeLine() const;

    //TODOoptional: private + getter/setter blah
    //TODOptional: private + getter/setter, so long as modifying list doesn't COW
    void setMethodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution(IDesignEqualsImplementationHaveOrderedListOfStatements *methodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution);
    IDesignEqualsImplementationHaveOrderedListOfStatements *methodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution() const;
    //QList<UseCaseEventListEntryType> OrderedUseCaseEvents; Come to think of it, slot provides an ordered list of statements, and THAT ALONE counts more or less as a "unit of execution". Additionally, it's probably easier to hack on that than to refactor everything to use Unit of Execution formal (but that refactor might be inevitible anyways)
private:
    IDesignEqualsImplementationHaveOrderedListOfStatements *m_MethodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution;
    DesignEqualsImplementationClassLifeLine *m_DesignEqualsImplementationClassLifeLine;
signals:
    void statementInserted(int indexInsertedInto, IDesignEqualsImplementationStatement *statementInserted);
};
#endif

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTION_H
