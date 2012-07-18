#ifndef RPCBANKSERVERCLIENTSHELPER_H
#define RPCBANKSERVERCLIENTSHELPER_H

#include <QObject>


#include "irpcbankserverbusiness.h"
#include "rpcbankserveractiondispensers.h"
#include "rpcbankserverbroadcastdispensers.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Actions/createbankaccountmessagedispenser.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Actions/getaddfundskeymessagedispenser.h"

#include "ibankserverprotocolknower.h"

class RpcBankServerClientsHelper : public QObject
{
    Q_OBJECT
public:
    explicit RpcBankServerClientsHelper(IRpcBankServerBusiness *rpcBankServer);
private:
    IRpcBankServerBusiness *m_RpcBankServer;
    RpcBankServerActionDispensers *m_ActionDispensers;
    RpcBankServerBroadcastDispensers *m_BroadcastDispensers;
    IBankServerProtocolKnower *m_Transporter;
    void takeOwnershipOfActionsAndSetupDelivery();

    CreateBankAccountMessageDispenser *m_CreateBankAccountMessageDispenser;
    GetAddFundsKeyMessageDispenser *m_GetAddFundsKeyMessageDispenser;
};

#endif // RPCBANKSERVERCLIENTSHELPER_H
