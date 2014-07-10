#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTION_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTION_H

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
    //QList<UseCaseEventListEntryType> OrderedUseCaseEvents; Come to think of it, slot provides an ordered list of statements, and THAT ALONE counts more or less as a "unit of execution". Additionally, it's probably easier to hack on that than to refactor everything to use Unit of Execution formal (but that refactor might be inevitible anyways)
private:
    DesignEqualsImplementationClassLifeLine *m_DesignEqualsImplementationClassLifeLine;
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTION_H
