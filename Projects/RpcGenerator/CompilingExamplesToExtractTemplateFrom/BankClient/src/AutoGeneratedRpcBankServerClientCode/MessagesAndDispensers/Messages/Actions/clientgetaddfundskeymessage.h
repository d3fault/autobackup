#ifndef CLIENTGETADDFUNDSKEYMESSAGE_H
#define CLIENTGETADDFUNDSKEYMESSAGE_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.h"
#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessagebase.h"

class ClientGetAddFundsKeyMessage : public IMessage, public GetAddFundsKeyMessageBase
{
    Q_OBJECT
public:
    explicit ClientGetAddFundsKeyMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // CLIENTGETADDFUNDSKEYMESSAGE_H
