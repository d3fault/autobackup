#ifndef SERVERCREATEBANKACCOUNTMESSAGE_H
#define SERVERCREATEBANKACCOUNTMESSAGE_H

#include "servercreatebankaccountmessageerrors.h"
#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessagebase.h"

#define CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(qds,strop) qds strop Success;
#define CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(qds,strop) qds strop Username;

class ServerCreateBankAccountMessage : public ServerCreateBankAccountMessageErrors, public CreateBankAccountMessageBase
{
    Q_OBJECT
public:
    ServerCreateBankAccountMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // SERVERCREATEBANKACCOUNTMESSAGE_H
