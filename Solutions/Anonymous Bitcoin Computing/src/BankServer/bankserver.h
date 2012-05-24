#ifndef BANKSERVER_H
#define BANKSERVER_H

#include <QObject>
#include <QThread>
#include <QDebug>

#include "bank.h"
#include "autoGendRpcClientHypothetical/rpcclientshelper.h"

class BankServer : public QObject
{
    Q_OBJECT
public:
    explicit BankServer(QObject *parent = 0);
    void startListening();
private:
    QThread *m_BankThread;
    Bank *m_Bank;
    RpcClientsHelper *m_RpcClientsHelper;
    QThread *m_ClientsThread;
signals:

private slots:
    void handleD(const QString &msg);
};

#endif // BANKSERVER_H
