#ifndef DESIGNEQUALSIMPLEMENTATIONLENIENTPROPERTYDECLARATIONPARSER_H
#define DESIGNEQUALSIMPLEMENTATIONLENIENTPROPERTYDECLARATIONPARSER_H

#include <QString>
#include <QList>

#include "libclangfunctiondeclarationparser.h" //just for ParsedTypeInstance::ParsedTypeInstanceCategoryEnum

class DesignEqualsImplementationLenientPropertyDeclarationParser
{
public:
    explicit DesignEqualsImplementationLenientPropertyDeclarationParser(const QString &propertyDeclaration, const QList<QString> &knownTypesToTypedefPrepend = QList<QString>());
    QString parsedPropertyQualifiedType() const;
    QString parsedPropertyUnqualifiedType() const;
    ParsedTypeInstance::ParsedTypeInstanceCategoryEnum parsedPropertyTypeInstanceCategory() const;
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

    QList<QString> m_KnownTypesToTypedefPrepend;

    QString m_ParsedPropertyQualifiedType;
    QString m_ParsedPropertyUnqualifiedType;
    ParsedTypeInstance::ParsedTypeInstanceCategoryEnum m_ParsedPropertyTypeInstanceCategory;
    QString m_ParsedPropertyName;
    bool m_HasInit;
    QString m_OptionalInit;
    QList<QString> m_UnknownTypesDetectedInLastRunToolOnCodeIteration;
    QList<QString> m_NewTypesSeenInPropertyDeclaration;

    bool m_HasError;
    QString m_MostRecentError; //could use queue/list/whatever, fuck it
};

#endif // DESIGNEQUALSIMPLEMENTATIONLENIENTPROPERTYDECLARATIONPARSER_H
