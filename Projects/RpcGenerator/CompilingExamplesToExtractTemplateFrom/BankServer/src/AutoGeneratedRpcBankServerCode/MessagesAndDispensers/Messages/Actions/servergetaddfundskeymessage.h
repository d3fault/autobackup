#ifndef SERVERGETADDFUNDSKEYMESSAGE_H
#define SERVERGETADDFUNDSKEYMESSAGE_H

#include "servergetaddfundskeymessageerrors.h"
#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessagebase.h"

#define GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(qds,strop) qds strop AddFundsKey;
#define GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(qds,strop) qds strop Username;

class ServerGetAddFundsKeyMessage : public ServerGetAddFundsKeyMessageErrors, public GetAddFundsKeyMessageBase
{
    Q_OBJECT
public:
    ServerGetAddFundsKeyMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // SERVERGETADDFUNDSKEYMESSAGE_H
