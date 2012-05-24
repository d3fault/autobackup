#ifndef RPCBANKSERVERNETWORKPROTOCOL_H
#define RPCBANKSERVERNETWORKPROTOCOL_H

#include <QDataStream>

struct RpcBankServerMessage
{
    enum RpcBankServerMessageType
    {
        InvalidRpcBankServerMessageType,
        RpcBankServerMessageActionType, //TODOreq: i currently have a contradiction. i want to use the same network message for a request/response but i DON'T want to needlessly send the username back in a response
        RpcBankServerMessageBroadcastType
    };
    quint8 m_MessageType;
    quint8 m_MessageAction; //maximum amount of actions has limit of 255 for a given request OR broadcast. can have 255 of each, m_MessageType is the switch

    //Auto-Generated, but re-used if the <property name= is the same. a lot of the <actions> will have a property with parameters with name "username", so we re-use it as the same QString... since a message can only be 1 action. TODO: make sure parameters don't have the same name. that is a rule of C++ anyways.
    //really, though... the stuff here should probably be called "string1,2,3" etc and there should be getter/setters in the implemented types for the setUsername and username(). they are auto-generated and so it is hardcoded what string they will use. per object, we only need as much strings as the action that uses the most strings has. this mildly inefficient as far as memory allocation goes (they re-using messages solves that problem)... AND the streaming operators take care of making sure unused strings are not sent over the network
    QString m_Username;
    double m_Amount;

    //only for RpcBankServerMessageRequestResponse
    //bool m_IsResponse; //why do i have an IsResponse? if the rpc client/appdb CODE _reads_ a RpcBankServerMessageRequestResponse, then it's obviously a response. it _writes_ the request. the inverse is true on the Bank. if it _reads_ then it's a request, and if it _writes_ then it is a response. so why would i put the value in the protocol if we can easily deduce that?

    bool m_ErrorOccurred; //it does still make sense to only use ErrorOccurred in the Response, however
    QString m_ErrorString; //only in message if(error-occured). lol premature ejaculation BUT THIS IS SO FUN (so is cumming). oh and also this is basically what protobuf does for you~ hah i am making an Rpc Generator as we speak. coding ABC simultaneously fuck yea

    bool isResponse; //not streamed, but used to tell whether or not we should stream m_ErrorOccurred or not. should be set to false until appropriate
    //theoretically isResponse should be a member of RpcBankServerMessageAction struct...but it isn't much of a waste of memory... and it doesn't get sent across the network for broadcasts. it allows us to use the same type for actions and broadcasts.... which allows us to have 1 network streaming code point
};

struct RpcBankServerMessageAction : public RpcBankServerMessage
{
    enum Action //Actions have both requests and responses
    {
        InvalidAction,
        CreateBankAccountAction,
        AddFundsKeyAction,
        BalanceTransferAction,
        DisburseAction
    };
};

struct RpcBankServerMessageBroadcast : public RpcBankServerMessage
{
    enum BroadcastMessage
    {
        InvalidBroadcastMessage,
        PendingAmountDetectedBroadcastMessage,
        ConfirmedAmountDetectedBroadcastMessage
    };
};

//so long as i keep these together and basically the exact same (except for the direction of the stream operator), i can do custom/efficient/minimal messaging. the in-memory message will not be efficient/minimal... only the network traffic :(. oh well network traffic is actually one of the most expensive things~
inline QDataStream &operator>>(QDataStream &in, RpcBankServerMessage &message)
{
    //the only benefit from having isResponse is that i can use the request that i read as the response. no copying is ever needed, i only change isResponse to true and set m_ErrorOccurred (which, during request phase, wasn't even streamed... though still allocated by us. tiny memory overhead saves us a message copy. it is ugly to have to set isResponse before using the stream operators TODO)
    //rpc client reads response from network - TODO: set isResponse to true before reading via stream operator (in the caller of this stream operator). since it's auto-generated code, it's not THAT big of a deal
    //rpc server reads request from network - TODO: see above todo about isResponse. same with with other stream operator below (during generation of these, i can really just string replace the "<<" and ">>" operators on a template lol...

    in >> message.m_MessageType;
    if(message.m_MessageType == RpcBankServerMessage::RpcBankServerMessageActionType)
    {
        in >> message.m_Username;

        if(message.isResponse)
        {
            in >> message.m_ErrorOccurred;
            if(message.m_ErrorOccurred)
            {
                in >> message.m_ErrorString;
            }
        }
    }

    //so basically this protocol in this header will return an object and set it's type. the also-autogenerated-code only reads the certain types based on whatever type it is. each type has a custom selection of what elements it uses... and that's defined in the interface as action parameters... so since it's strictly defined we don't need to do error checking to see if a member is set. if it is the type, it has the member. there should still be sanitization/sanity-checking somewhere <- TODO

    //TODO: .........username.............



    return in;
}
inline QDataStream &operator<<(QDataStream &out, RpcBankServerMessage &message)
{
    //rpc client writes request to network
    //rpc server writes response to network
    //TODO: also set isResponse to false for broadcasts i guess... maybe doesn't matter



    //it will be a fun challenge to get all of the switch cases auto-generated based on whether a type and subtype uses a certain parameter, based on whether it's defined in the interface or not
    //counting parameters in parsed xml, comparing names, etc

    if(message.m_MessageType != RpcBankServerMessage::InvalidRpcBankServerMessageType)
    {
        out << message.m_MessageType; //type = invalid, ActionMessageType, BroadcastMessageType
        out << message.m_MessageAction; //action = createbankaccount when

        switch(message.m_MessageType)
        {
        case RpcBankServerMessage::RpcBankServerMessageActionType:
        {
            //read in action-specific parameters
            switch(message.m_MessageAction)
            {
            case RpcBankServerMessageAction::CreateBankAccountAction:
                out << message.m_Username; //or, message.getUsername if it's backed by QString string0 etc
            break;
            }

            //read error stuffs if the message we are reading is known to be a response
            //broadcasts don't have responses, so they also don't have errors
            if(message.isResponse)
            {
                out << message.m_ErrorOccurred;
                if(message.m_ErrorOccurred)
                {
                    out << message.m_ErrorString;
                }
            }
        }
        break;
        case RpcBankServerMessage::RpcBankServerMessageBroadcastType:
            //read in action-specific params
            switch(message.m_MessageAction)
            {
            case RpcBankServerMessageBroadcast::PendingAmountDetectedBroadcastMessage:
            case RpcBankServerMessageBroadcast::ConfirmedAmountDetectedBroadcastMessage:
                out << message.m_Username;
                out << message.m_Amount;
            break;
            case RpcBankServerMessageBroadcast::InvalidBroadcastMessage:
            default:
                break;
            }
        break;
        case RpcBankServerMessage::InvalidRpcBankServerMessageType:
        default:
            break;
        }
    }
    else
    {
        //TODOreq: not sure if this makes sense here. we also don't want this propagating to the GUI
        //it probably does NOT belong here. why would we check the message right after we just streamed it to a QDataStream? it might make sense for the other stream. the extractor. might also make sense to use enums for the error type
        message.m_ErrorOccurred = true;
        message.m_ErrorString = "Invalid Broadcast Message Type";
    }
    return out;
}

#endif // RPCBANKSERVERNETWORKPROTOCOL_H
