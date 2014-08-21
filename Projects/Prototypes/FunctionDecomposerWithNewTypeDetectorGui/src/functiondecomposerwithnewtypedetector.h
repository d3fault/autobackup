#ifndef FUNCTIONDECOMPOSERWITHNEWTYPEDETECTOR_H
#define FUNCTIONDECOMPOSERWITHNEWTYPEDETECTOR_H

#include <QObject>
#include <QList>

class FunctionDecomposerWithNewTypeDetector : public QObject
{
    Q_OBJECT
public:
    explicit FunctionDecomposerWithNewTypeDetector(QObject *parent = 0);
private:
    friend class FunctionDecomposerDiagnosticConsumer;
    friend class RecursiveFunctionDecomposerVisitor;
    QList<QString> m_UnknownTypesDetectedThisIteration;
    QList<QString> m_TypesToBeCreated;
signals:
    void syntaxErrorDetected();
    void o(const QString &msg);
public slots:
    void parseFunctionDeclaration(const QString &functionDeclaration);
};

#endif // FUNCTIONDECOMPOSERWITHNEWTYPEDETECTOR_H
