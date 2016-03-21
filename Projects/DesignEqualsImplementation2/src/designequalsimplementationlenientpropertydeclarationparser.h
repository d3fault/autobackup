#ifndef DESIGNEQUALSIMPLEMENTATIONLENIENTPROPERTYDECLARATIONPARSER_H
#define DESIGNEQUALSIMPLEMENTATIONLENIENTPROPERTYDECLARATIONPARSER_H

#include <QString>
#include <QList>

class DesignEqualsImplementationLenientPropertyDeclarationParser
{
public:
    explicit DesignEqualsImplementationLenientPropertyDeclarationParser(const QString &propertyDeclaration, const QList<QString> &knownTypesToTypedefPrepend = QList<QString>());
    QString parsedPropertyQualifiedType() const;
    QString parsedPropertyUnqualifiedType() const;
    QString parsedPropertyName() const;
    bool hasInit() const;
    QString optionalInit() const;
    QList<QString> newTypesSeenInPropertyDeclaration() const;

    int m_NumEncounteredPropertyDeclarations;

    bool hasError() const;
    QString mostRecentError() const;
private:
    friend class DesignEqualsImplementationLenientPropertyDeclarationParserRecursiveAstVisitor;
    friend class DesignEqualsImplementationLenientPropertyDeclarationParserDiagnosticConsumer;

    QString m_ParsedPropertyQualifiedType;
    QString m_ParsedPropertyUnqualifiedType;
    QString m_ParsedPropertyName;
    bool m_HasInit;
    QString m_OptionalInit;
    QList<QString> m_UnknownTypesDetectedInLastRunToolOnCodeIteration;
    QList<QString> m_NewTypesSeenInPropertyDeclaration;

    bool m_HasError;
    QString m_MostRecentError; //could use queue/list/whatever, fuck it
};

#endif // DESIGNEQUALSIMPLEMENTATIONLENIENTPROPERTYDECLARATIONPARSER_H
