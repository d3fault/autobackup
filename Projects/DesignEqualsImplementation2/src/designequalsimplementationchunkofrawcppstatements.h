#ifndef DESIGNEQUALSIMPLEMENTATIONCHUNKOFRAWCPPSTATEMENTS_H
#define DESIGNEQUALSIMPLEMENTATIONCHUNKOFRAWCPPSTATEMENTS_H

#include "idesignequalsimplementationstatement.h"

#include <QStringList>

class DesignEqualsImplementationChunkOfRawCppStatements : public IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements
{
public:
    DesignEqualsImplementationChunkOfRawCppStatements(const QStringList &rawCppStatements = QStringList());
    QString toRawCppWithoutEndingSemicolon();
    void streamIn(DesignEqualsImplementationProject *project, QDataStream &in);
    void streamOut(DesignEqualsImplementationProject *project, QDataStream &out);
private:
    QStringList m_RawCppStatements;
};

#endif // DESIGNEQUALSIMPLEMENTATIONCHUNKOFRAWCPPSTATEMENTS_H
