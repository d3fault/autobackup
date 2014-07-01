#ifndef IDESIGNEQUALSIMPLEMENTATIONHAVEORDEREDLISTOFSTATEMENTS_H
#define IDESIGNEQUALSIMPLEMENTATIONHAVEORDEREDLISTOFSTATEMENTS_H

#include <QList>
#include <idesignequalsimplementationstatement.h>

class IDesignEqualsImplementationHaveOrderedListOfStatements
{
public:
    explicit IDesignEqualsImplementationHaveOrderedListOfStatements() { }
    virtual ~IDesignEqualsImplementationHaveOrderedListOfStatements() { qDeleteAll(OrderedListOfStatements); }
    QList<IDesignEqualsImplementationStatement*> OrderedListOfStatements;
};

#endif // IDESIGNEQUALSIMPLEMENTATIONHAVEORDEREDLISTOFSTATEMENTS_H
