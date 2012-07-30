#ifndef SERVERCONFIRMEDBALANCEDETECTEDMESSAGE_H
#define SERVERCONFIRMEDBALANCEDETECTEDMESSAGE_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.h"
#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/confirmedbalancedetectedmessagebase.h"

class ServerConfirmedBalanceDetectedMessage : public IMessage, public ConfirmedBalanceDetectedMessageBase
{
    Q_OBJECT
public:
    ServerConfirmedBalanceDetectedMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // SERVERCONFIRMEDBALANCEDETECTEDMESSAGE_H
