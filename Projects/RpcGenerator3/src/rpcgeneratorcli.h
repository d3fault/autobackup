#ifndef RPCGENERATORCLI_H
#define RPCGENERATORCLI_H

#include <QObject>

#include <QTextStream>

class RpcGeneratorCli : public QObject
{
    Q_OBJECT
public:
    explicit RpcGeneratorCli(QObject *parent = 0);
private:
    QTextStream m_StdOut;
    QTextStream m_StdErr;
signals:
    void generateRpcRequested();
    void exitRequested(int exitCode);
private slots:
    void handleO(QString msg);
    void handleE(QString msg);
    void handleRpcGenerated(bool success = false, const QString &directoryFilesWereGeneratedInto = QString());
};

#endif // RPCGENERATORCLI_H
