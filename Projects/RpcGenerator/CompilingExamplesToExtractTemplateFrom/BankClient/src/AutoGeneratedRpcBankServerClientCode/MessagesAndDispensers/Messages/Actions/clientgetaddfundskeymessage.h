#if 0
#ifndef CLIENTGETADDFUNDSKEYMESSAGE_H
#define CLIENTGETADDFUNDSKEYMESSAGE_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.h"
#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessagebase.h"

class GetAddFundsKeyMessage : public IMessage, public GetAddFundsKeyMessageBase
{
    Q_OBJECT
public:
    explicit GetAddFundsKeyMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // CLIENTGETADDFUNDSKEYMESSAGE_H
#endif
