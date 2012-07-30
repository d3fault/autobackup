#ifndef SERVERCREATEBANKACCOUNTMESSAGE_H
#define SERVERCREATEBANKACCOUNTMESSAGE_H

#include "servercreatebankaccountmessageerrors.h"
#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessagebase.h"

class ServerCreateBankAccountMessage : public ServerCreateBankAccountMessageErrors, public CreateBankAccountMessageBase
{
    Q_OBJECT
public:
    ServerCreateBankAccountMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // SERVERCREATEBANKACCOUNTMESSAGE_H
