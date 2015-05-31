#include "rpcgeneratorcli.h"

#include <QCoreApplication>

#include "rpcgenerator.h"

RpcGeneratorCli::RpcGeneratorCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{
    RpcGenerator *rpcGenerator = new RpcGenerator(this);

    connect(rpcGenerator, &RpcGenerator::o, this, &RpcGeneratorCli::handleO);
    connect(rpcGenerator, &RpcGenerator::e, this, &RpcGeneratorCli::handleE);
    connect(rpcGenerator, &RpcGenerator::rpcGenerated, this, &RpcGeneratorCli::handleRpcGenerated);
    connect(this, &RpcGeneratorCli::generateRpcRequested, rpcGenerator, &RpcGenerator::generateRpc);
    connect(this, &RpcGeneratorCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection);

    emit generateRpcRequested();
}
void RpcGeneratorCli::handleO(QString msg)
{
    m_StdOut << msg << endl;
}
void RpcGeneratorCli::handleE(QString msg)
{
    m_StdErr << msg << endl;
}
void RpcGeneratorCli::handleRpcGenerated(bool success, const QString &directoryFilesWereGeneratedInto)
{
    if(!success)
        handleE("failed to generate rpc");
    handleO(directoryFilesWereGeneratedInto);
    emit exitRequested(success ? 0 : 1);
}
