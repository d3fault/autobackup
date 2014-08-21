#ifndef DESIGNEQUALSIMPLEMENTATIONFUNCTIONDECLARATIONPARSER_H
#define DESIGNEQUALSIMPLEMENTATIONFUNCTIONDECLARATIONPARSER_H

#include <QString>
#include <QList>

class DesignEqualsImplementationFunctionDeclarationParser
{
public:
    DesignEqualsImplementationFunctionDeclarationParser(const QString &functionDeclaration, const QList<QString> &knownTypesToTypedefPrepend = QList<QString>());
    bool hasUnrecoverableSyntaxError() const;
    QString parsedFunctionName() const;
    QList<QString> newTypesSeenInFunctionDeclaration() const;
private:
    friend class DesignEqualsImplementationFunctionDeclarationParserRecursiveAstVisitor;
    friend class DesignEqualsImplementationFunctionDeclarationDiagnosticConsumer;

    QString m_OriginalFunctionDeclaration;
    bool m_HasUnrecoverableSyntaxError;
    QString m_ParsedFunctionName;
    QList<QString> m_UnknownTypesDetectedInLastRunToolOnCodeIteration;
    QList<QString> m_NewTypesSeenInFunctionDeclaration;
};

#endif // DESIGNEQUALSIMPLEMENTATIONFUNCTIONDECLARATIONPARSER_H
