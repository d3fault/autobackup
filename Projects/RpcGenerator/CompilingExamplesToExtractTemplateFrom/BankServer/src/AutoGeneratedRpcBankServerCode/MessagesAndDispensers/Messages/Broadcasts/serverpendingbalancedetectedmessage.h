#ifndef SERVERPENDINGBALANCEDETECTEDMESSAGE_H
#define SERVERPENDINGBALANCEDETECTEDMESSAGE_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.h"
#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/pendingbalancedetectedmessagebase.h"

class ServerPendingBalanceDetectedMessage : public IMessage, public PendingBalanceDetectedMessageBase
{
    Q_OBJECT
public:
    ServerPendingBalanceDetectedMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);

    void resetMessageParameters();
};

#endif // SERVERPENDINGBALANCEDETECTEDMESSAGE_H
