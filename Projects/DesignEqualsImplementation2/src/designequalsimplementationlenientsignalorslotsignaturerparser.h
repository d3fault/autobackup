#ifndef DESIGNEQUALSIMPLEMENTATIONLENIENTSIGNALORSLOTSIGNATUREPARSER_H
#define DESIGNEQUALSIMPLEMENTATIONLENIENTSIGNALORSLOTSIGNATUREPARSER_H

#include <QString>
#include <QList>

#include "designequalsimplementationclass.h"

class DesignEqualsImplementationLenientSignalOrSlotSignaturerParser
{
public:
    explicit DesignEqualsImplementationLenientSignalOrSlotSignaturerParser(const QString &functionDeclaration, const QList<QString> &knownTypesToTypedefPrepend = QList<QString>());
    QString parsedFunctionName() const;
    QList<QString> newTypesSeenInFunctionDeclaration() const;
    QList<FunctionArgumentTypedef> parsedFunctionArguments() const;

    int m_NumEncounteredFunctionDeclarations;

    bool hasError() const;
    QString mostRecentError() const;
private:
    friend class DesignEqualsImplementationLenientSignalOrSlotSignaturerParserRecursiveAstVisitor;
    friend class DesignEqualsImplementationLenientSignalOrSlotSignaturerParserDiagnosticConsumer;

    QString m_ParsedFunctionName;
    QList<QString> m_UnknownTypesDetectedInLastRunToolOnCodeIteration;
    QList<QString> m_NewTypesSeenInFunctionDeclaration;
    QList<FunctionArgumentTypedef> m_ParsedFunctionArguments;

    bool m_HasError;
    QString m_MostRecentError; //could use queue/list/whatever, fuck it
};

#endif // DESIGNEQUALSIMPLEMENTATIONLENIENTSIGNALORSLOTSIGNATUREPARSER_H
