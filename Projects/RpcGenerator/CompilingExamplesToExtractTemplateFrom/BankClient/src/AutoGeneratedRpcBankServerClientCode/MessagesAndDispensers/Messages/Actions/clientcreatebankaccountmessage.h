#ifndef CLIENTCREATEBANKACCOUNTMESSAGE_H
#define CLIENTCREATEBANKACCOUNTMESSAGE_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.h"
#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessagebase.h"

class ClientCreateBankAccountMessage : public IMessage, public CreateBankAccountMessageBase
{
    Q_OBJECT
public:
    explicit ClientCreateBankAccountMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // CLIENTCREATEBANKACCOUNTMESSAGE_H
