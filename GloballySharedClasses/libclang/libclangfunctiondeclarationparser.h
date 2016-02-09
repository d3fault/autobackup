#ifndef LIBCLANGFUNCTIONDECLARATIONPARSER_H
#define LIBCLANGFUNCTIONDECLARATIONPARSER_H

#include <QString>
#include <QList>
#include <QPair>

typedef QPair<QString /*arg type*/, QString /*arg name*/> FunctionArgumentTypedef;

class LibClangFunctionDeclarationParser
{
public:
    explicit LibClangFunctionDeclarationParser();
    explicit LibClangFunctionDeclarationParser(const QString &functionDeclaration, const QList<QString> &knownTypesToTypedefPrepend = QList<QString>());
    void reset();
    void parseFunctionDeclaration(const QString &functionDeclaration, const QList<QString> &knownTypesToTypedefPrepend = QList<QString>());
    QString parsedFunctionName() const;
    QList<QString> newTypesSeenInFunctionDeclaration() const;
    QList<FunctionArgumentTypedef> parsedFunctionArguments() const;

    bool hasError() const;
    QString mostRecentError() const;
private:
    friend class LibClangFunctionDeclarationParserRecursiveAstVisitor;
    friend class LibClangFunctionDeclarationParserDiagnosticConsumer;

    int m_NumEncounteredFunctionDeclarations;
    QString m_ParsedFunctionName;
    QList<QString> m_UnknownTypesDetectedInLastRunToolOnCodeIteration;
    QList<QString> m_NewTypesSeenInFunctionDeclaration;
    QList<FunctionArgumentTypedef> m_ParsedFunctionArguments;

    bool m_HasError;
    QString m_MostRecentError; //could use queue/list/whatever, fuck it
};

#endif // LIBCLANGFUNCTIONDECLARATIONPARSER_H
