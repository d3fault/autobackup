#ifndef RPCBANKSERVERCLIENTSHELPER_H
#define RPCBANKSERVERCLIENTSHELPER_H

#include <QObject>

#include "../../../RpcBankServerAndClientShared/irpcbankserver.h"
#include "../../../RpcBankServerAndClientShared/actiondispensers.h"
#include "../../../RpcBankServerAndClientShared/broadcastdispensers.h"

class RpcBankServerClientsHelper : public QObject
{
    Q_OBJECT
public:
    explicit RpcBankServerClientsHelper(IRpcBankServer *rpcBankServer);
private:
    IRpcBankServer *m_RpcBankServer;
    ActionDispensers *m_ActionDispensers;
    BroadcastDispensers *m_BroadcastDispensers;
signals:

public slots:

};

#endif // RPCBANKSERVERCLIENTSHELPER_H
