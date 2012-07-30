#ifndef CLIENTCONFIRMEDBALANCEDETECTEDMESSAGE_H
#define CLIENTCONFIRMEDBALANCEDETECTEDMESSAGE_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/irecycleableandstreamable.h"
#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/confirmedbalancedetectedmessagebase.h"

class ClientConfirmedBalanceDetectedMessage : public IRecycleableAndStreamable, public ConfirmedBalanceDetectedMessageBase
{
    Q_OBJECT
public:
    explicit ClientConfirmedBalanceDetectedMessage(QObject *parent);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // CLIENTCONFIRMEDBALANCEDETECTEDMESSAGE_H
