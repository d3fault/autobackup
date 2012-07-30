#ifndef SERVERCONFIRMEDBALANCEDETECTEDMESSAGE_H
#define SERVERCONFIRMEDBALANCEDETECTEDMESSAGE_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.h"
#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/confirmedbalancedetectedmessagebase.h"

//it is still beneficial for broadcasts to use this define so there is still only 1 official definition (not that it matters since we're generated code)
#define CONFIRMED_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(qds,strop) qds strop Username; qds strop ConfirmedBalance;

class ServerConfirmedBalanceDetectedMessage : public IMessage, public ConfirmedBalanceDetectedMessageBase
{
    Q_OBJECT
public:
    ServerConfirmedBalanceDetectedMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // SERVERCONFIRMEDBALANCEDETECTEDMESSAGE_H
