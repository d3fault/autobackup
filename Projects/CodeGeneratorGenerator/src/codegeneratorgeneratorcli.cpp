#include "codegeneratorgeneratorcli.h"

#include <QCoreApplication>

CodeGeneratorGeneratorCli::CodeGeneratorGeneratorCli(QObject *parent)
    : QObject(parent)
    , m_StdErr(stderr)
    , m_StdOut(stdout)
{
    connect(this, &CodeGeneratorGeneratorCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection);
}
void CodeGeneratorGeneratorCli::handleE(QString msg)
{
    m_StdErr << msg << endl;
}
void CodeGeneratorGeneratorCli::handleO(QString msg)
{
    m_StdOut << msg << endl;
}
void CodeGeneratorGeneratorCli::handleV(QString msg)
{
    handleO(msg);
}
void CodeGeneratorGeneratorCli::handleGenerateCodeGeneratorFinished(bool success, QString outputDir)
{
    handleO("handleGenerateCodeGeneratorFinished " + QString(success ? "" : "UN") + "successfully");
    if(success)
        handleO("output dir: " + outputDir);
    emit exitRequested(success ? 0 : 1);
}
