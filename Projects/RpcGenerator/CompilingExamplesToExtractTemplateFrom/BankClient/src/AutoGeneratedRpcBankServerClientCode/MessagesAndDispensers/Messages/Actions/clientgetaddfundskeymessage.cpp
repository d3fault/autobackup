#if 0
#include "clientgetaddfundskeymessage.h"

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessagebase.h"

GetAddFundsKeyMessage::GetAddFundsKeyMessage(QObject *owner)
    : IMessage(owner)
{ }
void GetAddFundsKeyMessage::streamIn(QDataStream &in)
{
    GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(in,>>);
}
void GetAddFundsKeyMessage::streamOut(QDataStream &out)
{
    GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(out,<<);
}
#endif
