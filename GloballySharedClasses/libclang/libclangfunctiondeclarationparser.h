#ifndef LIBCLANGFUNCTIONDECLARATIONPARSER_H
#define LIBCLANGFUNCTIONDECLARATIONPARSER_H

#include <QString>
#include <QList>
#include <QPair>

struct ParsedTypeInstance
{
    enum ParsedTypeInstanceCategoryEnum
    {
          KnownTypeButNotABuiltIn
        , BuiltIn
        , Unknown
    };

    QString NonQualifiedType;
    QString QualifiedType;
    QString Name;
    ParsedTypeInstanceCategoryEnum ParsedTypeInstanceCategory;
};
class LibClangFunctionDeclarationParser
{
public:
    explicit LibClangFunctionDeclarationParser();
    explicit LibClangFunctionDeclarationParser(const QString &functionDeclaration, const QList<QString> &knownTypesToTypedefPrepend = QList<QString>());
    void reset();
    void parseFunctionDeclaration(const QString &functionDeclaration, const QList<QString> &knownTypesToTypedefPrepend = QList<QString>());
    QString parsedFunctionName() const;
    QList<QString> newTypesSeenInFunctionDeclaration() const;
    QList<ParsedTypeInstance> parsedFunctionArguments() const;

    bool hasError() const;
    QString mostRecentError() const;
private:
    friend class LibClangFunctionDeclarationParserRecursiveAstVisitor;
    friend class LibClangFunctionDeclarationParserDiagnosticConsumer;

    QList<QString> m_KnownTypesToTypedefPrepend;
    int m_NumEncounteredFunctionDeclarations;
    QString m_ParsedFunctionName;
    QList<QString> m_UnknownTypesDetectedInLastRunToolOnCodeIteration;
    QList<QString> m_NewTypesSeenInFunctionDeclaration;
    QList<ParsedTypeInstance> m_ParsedFunctionArguments;

    bool m_HasError;
    QString m_MostRecentError; //could use queue/list/whatever, fuck it
};

#endif // LIBCLANGFUNCTIONDECLARATIONPARSER_H
