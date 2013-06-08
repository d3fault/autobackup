#ifndef RPCBANKSERVERCLIENTPROTOCOLKNOWER_H
#define RPCBANKSERVERCLIENTPROTOCOLKNOWER_H

#include <QHash>

#include "../../../RpcBankServerAndClientShared/iacceptrpcbankserveractiondeliveries.h"
#include "iprotocolknower.h"

#include "../../RpcBankServerAndClientShared/rpcbankserverheader.h"
#include "../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/iactionmessage.h"
#include "../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.h"
#include "../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessage.h"

#include "MessagesAndDispensers/Dispensers/Broadcasts/clientpendingbalancedetectedmessagedispenser.h"
#include "MessagesAndDispensers/Dispensers/Broadcasts/clientconfirmedbalancedetectedmessagedispenser.h"


//signal relay hack
#include "iacceptrpcbankserverclientactiondeliveries_and_iemitactionsandbroadcastsbothwithmessageasparamforsignalrelayhack.h"

//TODOreq: lazy ack ack is definitely an awesome and effective optimization.... for high volume servers... but what if a low volume server is using it. My point is, and this will take a lot of consideration: do I need to manually ack the action responses (or whatever else is using 'lazy ack ack' scheme (maybe broadcasts too but i forget lol)) after a certain amount of time on low volume servers? Does it have any negative effect to have the action responses sitting their awaiting their lazy ack for a long amount of time? I'm actually leaning towards "it doesn't fucking matter"... but could be wrong heh.
//Yes I think broadcasts do use lazy ack ack scheme. The lazy ack rides on the next broadcast to ack the 'current' (previous relative to the just mentioned 'next') broadcast's ack.

class RpcBankServerClientProtocolKnower
        : public IAcceptRpcBankServerActionDeliveries, public IProtocolKnower
{
    Q_OBJECT
public:
    static void setSignalRelayHackEmitter(IAcceptRpcBankServerClientActionDeliveries_AND_IEmitActionsAndBroadcastsBothWithMessageAsParamForSignalRelayHack *signalRelayHackEmitter) { m_SignalRelayHackEmitter = signalRelayHackEmitter; }
    explicit RpcBankServerClientProtocolKnower(QObject *parent = 0);
    virtual void messageReceived();
private:
    //TODOoptimization: could put streamToByteArrayAndTransmitToServer in... IAcceptRpcBankServerActionDeliveries and have Client/Server using the same piece of code. It "makes more sense" for it to go into IProtocolKnower, BUT he doesn't know IMessage. I think it's really just a coincidence that both the client/server versions of 'this' inherit from IAcceptRpcBankServerActionDeliveries, heh...
    inline void streamToByteArrayAndTransmitToServer(IMessage *message) //Old: Server accepts IMessage here because it needs to be able to transmit actions AND broadcasts... but client only ever needs to transmit actions (broadcast acks might change this though!!! that's tbd) so that's why it's IMessage. Wouldn't be a big deal if I changed it back to IMessage lol... doesn't matter... So wait why the fuck am I writing this comment? Changing back to IMessage.
    {
        //stream to byte array
        resetTransmitMessage();

        //m_AbstractClientConnection->streamDoneHelloingFromServerIntoMessageAboutToBeTransmitted(&m_TransmitMessageByteArray); //pretty sure i can now conclude that "streaming into" a message does not clear/overwrite it. it wouldn't make any sense if it did... how could i stream multiple items into a single qba if that was the case. OR PERHAPS the question was more relating to streaming out of (reading) a QDS and into a singular.... POD (QString counts). does that POD first need resetting? TODOreq (ignore first part of comment lol, it's already handled)

        streamDoneHelloingIntoMessageAboutToBeTransmitted();

        //after writing the following line, i just realize that i need to use the appropriate enums in places like... 'here it is again bitch' TODOreq... and probably lots of other places too! that "enum for-each 'find usages'" op should solve this implicitly as well...
        m_TransmitMessageDataStream << message->Header; //TODOreq: actions pretty much just send the same header back (after updating the GenericRpcType of course), but Broadcasts need to fill out the entire header manually
        m_TransmitMessageDataStream << *message;

        //transmit
        m_AbstractConnection->transmitMessage(&m_TransmitMessageByteArray);
    }

    static IAcceptRpcBankServerClientActionDeliveries_AND_IEmitActionsAndBroadcastsBothWithMessageAsParamForSignalRelayHack *m_SignalRelayHackEmitter;

    //Dispensers
    ClientPendingBalanceDetectedMessageDispenser *m_PendingBalanceDetectedMessageDispenser;
    ClientConfirmedBalanceDetectedMessageDispenser *m_ConfirmedBalanceDetectedMessageDispenser;

    //Action Requests that have been sent to the server (or so we think. the network transmission may have failed)
    QHash<quint32 /*MessageId*/, IActionMessage* /*pending-in-business*/> m_CreateBankAccountMessagesPendingResponseFromServer;
    QHash<quint32 /*MessageId*/, IActionMessage* /*pending-in-business*/> m_GetAddFundsKeyMessagesPendingResponseFromServer;
    //etc for each Action
public slots:
    void createBankAccountDelivery();
    void getAddFundsKeyDelivery();
};

#endif // RPCBANKSERVERCLIENTPROTOCOLKNOWER_H
