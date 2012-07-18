#ifndef RPCBANKSERVERCLIENTSHELPER_H
#define RPCBANKSERVERCLIENTSHELPER_H

#include <QObject>

#include "../../../RpcBankServerAndClientShared/irpcbankserver.h"
#include "../../../RpcBankServerAndClientShared/broadcastdispensers.h"

class RpcBankServerClientsHelper : public QObject
{
    Q_OBJECT
public:
    explicit RpcBankServerClientsHelper(IRpcBankServer *rpcBankServer);
private:
    IRpcBankServer *m_RpcBankServer;
signals:

public slots:

};

#endif // RPCBANKSERVERCLIENTSHELPER_H
