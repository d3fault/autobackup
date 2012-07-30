#include "servergetaddfundskeymessage.h"

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessagebase.h"

ServerGetAddFundsKeyMessage::ServerGetAddFundsKeyMessage(QObject *owner)
    : ServerGetAddFundsKeyMessageErrors(owner)
{ }
void ServerGetAddFundsKeyMessage::streamIn(QDataStream &in)
{
    //these can probably be defines because i can/will/do know at compile time what role i am. saves me from having to do an m_IsResponse and constant checking of it. basically just reverse the roles for the client/server methinks. these defines actually have a benefit: only one place to define protocol (not that it matters since i'm a code generator)

    GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(in,>>);
}
void ServerGetAddFundsKeyMessage::streamOut(QDataStream &out)
{
    GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(out,<<);

    //now that i think of it, these defines are not all that handy because we are a code generator. but still, i am writing this for reals right now... and it is handy methinks. if it gets in the way just tear it down. just like IAccept* is about to be destroyed. that's irrelevant i shouldn't blog/talk in comments randomly. committing before destorying IAccept*
}
