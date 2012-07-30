#ifndef CLIENTPENDINGBALANCEDETECTEDMESSAGE_H
#define CLIENTPENDINGBALANCEDETECTEDMESSAGE_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/irecycleableandstreamable.h"
#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/pendingbalancedetectedmessagebase.h"

class ClientPendingBalanceDetectedMessage : public IRecycleableAndStreamable, public PendingBalanceDetectedMessageBase
{
    Q_OBJECT
public:
    explicit ClientPendingBalanceDetectedMessage(QObject *parent = 0);
    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // CLIENTPENDINGBALANCEDETECTEDMESSAGE_H
