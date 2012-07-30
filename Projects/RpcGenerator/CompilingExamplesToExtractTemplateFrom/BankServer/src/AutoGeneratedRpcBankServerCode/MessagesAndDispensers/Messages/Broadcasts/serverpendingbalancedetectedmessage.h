#ifndef SERVERPENDINGBALANCEDETECTEDMESSAGE_H
#define SERVERPENDINGBALANCEDETECTEDMESSAGE_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.h"
#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/pendingbalancedetectedmessagebase.h"

#define PENDING_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(qds,strop) qds strop Username; qds strop PendingBalance;

class ServerPendingBalanceDetectedMessage : public IMessage, public PendingBalanceDetectedMessageBase
{
    Q_OBJECT
public:
    ServerPendingBalanceDetectedMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // SERVERPENDINGBALANCEDETECTEDMESSAGE_H
