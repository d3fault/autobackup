#ifndef DESIGNEQUALSIMPLEMENTATIONCHUNKOFRAWCPPSTATEMENTS_H
#define DESIGNEQUALSIMPLEMENTATIONCHUNKOFRAWCPPSTATEMENTS_H

#include "idesignequalsimplementationstatement.h"

#include <QStringList>

class DesignEqualsImplementationChunkOfRawCppStatements : public IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements
{
public:
    DesignEqualsImplementationChunkOfRawCppStatements();
private:
    QStringList m_RawCppStatements;
};

#endif // DESIGNEQUALSIMPLEMENTATIONCHUNKOFRAWCPPSTATEMENTS_H
