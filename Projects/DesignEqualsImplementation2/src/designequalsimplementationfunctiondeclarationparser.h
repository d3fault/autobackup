#ifndef DESIGNEQUALSIMPLEMENTATIONFUNCTIONDECLARATIONPARSER_H
#define DESIGNEQUALSIMPLEMENTATIONFUNCTIONDECLARATIONPARSER_H

#include <QObject>
#include <QList>

#include "designequalsimplementationclass.h"

class DesignEqualsImplementationFunctionDeclarationParser : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationFunctionDeclarationParser(const QString &functionDeclaration, const QList<QString> &knownTypesToTypedefPrepend = QList<QString>(), QObject *parent = 0);
    bool hasUnrecoverableSyntaxError() const;
    QString parsedFunctionName() const;
    QList<QString> newTypesSeenInFunctionDeclaration() const;
    QList<MethodArgumentTypedef> parsedFunctionArguments() const;
private:
    friend class DesignEqualsImplementationFunctionDeclarationParserRecursiveAstVisitor;
    friend class DesignEqualsImplementationFunctionDeclarationDiagnosticConsumer;

    QString m_OriginalFunctionDeclaration;
    bool m_HasUnrecoverableSyntaxError;
    QString m_ParsedFunctionName;
    QList<QString> m_UnknownTypesDetectedInLastRunToolOnCodeIteration;
    QList<QString> m_NewTypesSeenInFunctionDeclaration;
    QList<MethodArgumentTypedef> m_ParsedFunctionArguments;
signals:
    void e(const QString &msg);
};

#endif // DESIGNEQUALSIMPLEMENTATIONFUNCTIONDECLARATIONPARSER_H
