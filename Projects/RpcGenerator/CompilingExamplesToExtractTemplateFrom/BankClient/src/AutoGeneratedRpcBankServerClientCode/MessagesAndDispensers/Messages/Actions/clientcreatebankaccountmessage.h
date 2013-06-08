#if 0
#ifndef ClientCreateBankAccountMessage_H
#define ClientCreateBankAccountMessage_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.h"
#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessagebase.h"
#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.h"

class ClientCreateBankAccountMessage : public IMessage, public CreateBankAccountMessageBase
{
    Q_OBJECT
public:
    explicit ClientCreateBankAccountMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // ClientCreateBankAccountMessage_H
#endif
