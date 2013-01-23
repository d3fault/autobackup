#ifndef RPCBANKSERVERPROTOCOLKNOWER_H
#define RPCBANKSERVERPROTOCOLKNOWER_H

#include "iprotocolknower.h"

#include "../../RpcBankServerAndClientShared/rpcbankserverheader.h"

#include "iemitrpcbankserveractionrequestsignalswithmessageasparamandiacceptactiondeliveries.h"
#include "iprotocolknower.h"
#include "MessagesAndDispensers/Dispensers/Actions/servercreatebankaccountmessagedispenser.h"

class RpcBankServerProtocolKnower : public IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries, public IProtocolKnower
{
    Q_OBJECT
public:
    explicit RpcBankServerProtocolKnower(QObject *parent);
private:
    ServerCreateBankAccountMessageDispenser *m_CreateBankAccountMessageDispenser;
protected:
    virtual void messageReceived(QDataStream *messageDataStream);
};

#endif // RPCBANKSERVERPROTOCOLKNOWER_H
