#include "clientgetaddfundskeymessage.h"

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessagebase.h"

ClientGetAddFundsKeyMessage::ClientGetAddFundsKeyMessage(QObject *owner)
    : IMessage(owner)
{ }
void ClientGetAddFundsKeyMessage::streamIn(QDataStream &in)
{
    GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(in,>>);
}
void ClientGetAddFundsKeyMessage::streamOut(QDataStream &out)
{
    GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(out,<<);
}
