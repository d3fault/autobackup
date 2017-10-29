#ifndef CODEGENERATORGENERATORCLI_H
#define CODEGENERATORGENERATORCLI_H

#include <QObject>

#include <QTextStream>

class CodeGeneratorGeneratorCli : public QObject
{
    Q_OBJECT
public:
    explicit CodeGeneratorGeneratorCli(QObject *parent = nullptr);
private:
    QTextStream m_StdErr;
    QTextStream m_StdOut;
signals:
    void generateCodeGeneratorRequested(QString directoryToCompilingExampleTemplateToGenerateCodeGeneratorFor);
    void exitRequested(int posixExitCode);
public slots:
    void handleE(QString msg);
    void handleO(QString msg);
    void handleV(QString msg);
    void handleGenerateCodeGeneratorFinished(bool success, QString outputDir);
};

#endif // CODEGENERATORGENERATORCLI_H
