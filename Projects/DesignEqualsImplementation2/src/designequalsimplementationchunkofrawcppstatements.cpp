#include "designequalsimplementationchunkofrawcppstatements.h"

DesignEqualsImplementationChunkOfRawCppStatements::DesignEqualsImplementationChunkOfRawCppStatements(const QStringList &rawCppStatements)
    : IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements(IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements::ChunkOfRawCppStatements)
    , m_RawCppStatements(rawCppStatements)
{ }
QString DesignEqualsImplementationChunkOfRawCppStatements::toRawCppWithoutEndingSemicolon()
{
    QString ret;
    Q_FOREACH(const QString &currentRawCppStatement, m_RawCppStatements)
    {
        ret.append(currentRawCppStatement + "\n");
    }
    return ret;
}
void DesignEqualsImplementationChunkOfRawCppStatements::streamIn(DesignEqualsImplementationProject *project, QDataStream &in)
{
    Q_UNUSED(project)
    in >> m_RawCppStatements;
}
void DesignEqualsImplementationChunkOfRawCppStatements::streamOut(DesignEqualsImplementationProject *project, QDataStream &out)
{
    Q_UNUSED(project)
    out << m_RawCppStatements;
}
