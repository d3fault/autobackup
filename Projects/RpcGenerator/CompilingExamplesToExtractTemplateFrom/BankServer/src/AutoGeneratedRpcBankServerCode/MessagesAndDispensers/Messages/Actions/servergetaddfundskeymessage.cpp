#include "servergetaddfundskeymessage.h"

ServerGetAddFundsKeyMessage::ServerGetAddFundsKeyMessage(QObject *owner)
    : ServerGetAddFundsKeyMessageErrors(owner)
{ }
void ServerGetAddFundsKeyMessage::streamIn(QDataStream &in)
{

    //TODO LEFT OFF
    //fuck, for the rpc client -> server we'll be streaming OUT, but we want to use the 'input params' Username, not AddFundsKey. i need to waste protocol space in order to make this easy... but i think there's a better solution somehow, i just haven't thought of it yet
    /*
    if(m_IsResponse)
    {
        //rpc client reading in response from network

        in >> AddFundsKey;
    }
    else
    {
        //rpc server reading in request from network

        //input 'params'
        in >> Username;
    }
    */

    //these can probably be defines because i can/will/do know at compile time what role i am. saves me from having to do an m_IsResponse and constant checking of it. basically just reverse the roles for the client/server methinks. these defines actually have a benefit: only one place to define protocol (not that it matters since i'm a code generator)
#ifdef RPC_BANK_SERVER_CLIENT_CODE
    GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(in,>>);
#else
    GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(in,>>);
#endif
}
void ServerGetAddFundsKeyMessage::streamOut(QDataStream &out)
{
    /*
    if(m_IsResponse)
    {
        //rpc server writing response to network

        //return type or 'out' params
        out << AddFundsKey;
    }
    else
    {
        //rpc client writing request to network

        out << Username;
    }
    */

#ifdef RPC_BANK_SERVER_CODE
    GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(out,<<);
#else
    GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(out,<<);
#endif

    //now that i think of it, these defines are not all that handy because we are a code generator. but still, i am writing this for reals right now... and it is handy methinks. if it gets in the way just tear it down. just like IAccept* is about to be destroyed. that's irrelevant i shouldn't blog/talk in comments randomly. committing before destorying IAccept*
}
