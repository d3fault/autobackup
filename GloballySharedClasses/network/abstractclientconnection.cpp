#include "abstractclientconnection.h"

AbstractClientConnection::AbstractClientConnection(QIODevice *ioDeviceToClient, QObject *parent)
    : QObject(parent), m_IoDeviceToClient(ioDeviceToClient), m_DataStreamToClient(ioDeviceToClient), m_NetworkMagic(ioDeviceToClient), m_ServerHelloState(AwaitingHello), m_HasCookie(false)
{
    m_TheMessageBuffer.setBuffer(&m_TheMessageByteArray);
    m_TheMessageBuffer.open(QIODevice::ReadWrite);
    m_TheMessageDataStream.setDevice(&m_TheMessageBuffer);
}
void AbstractClientConnection::setServerBusiness(IMultiServerBusiness *serverBusiness)
{
    m_ServerBusiness = serverBusiness;
}
void AbstractClientConnection::potentialMergeCaseAsCookieIsSupplied()
{
    //At this point, our cookie is set with the client-supplied one, but we are not done with the hello phase (we are right int he middle of it!)... so we are not on m_ListOfHelloedConnections (which means we can check against it)

    AbstractClientConnection *toMergeWithMaybe = getAddressOfConnectionUsingCookie__OrZero(cookie());
    if(toMergeWithMaybe)
    {
        //merge. fuck, this seems like it should be a lot more complicated than a toMergeWithMaybe.setIoDevice(this.getIODevice), followed by a deletion of 'this' (deleteLater methinks?). Oh and I'd have to do the slot stuff too and also delete the original iodevice being overwritten???? I wonder if it's implicitly shared and has other references? Is this a security issue letting clients easily override/merge-with each other? It ultimately depends on the ssl cert being leaked i guess, or if you are dumb and choose TCP over WAN (in which case your shit can be hacked anyways~, even if you use a more elaborate username/password scheme). cert comrpomise means machine compromise means username/password compromise means fuck doing it (username/password) to begin with? I'm getting way away from the point. How the fuck do I merge two connections? This is where I keep getting stuck. I have to factor in shit like pending messages. Note that the cookie scheme is not a security measure, just an identification measure. It abstracts the connection types, because I can't use like IP address and shit for identification
        //also not sure if the server abstraction or myself should do the merging. i know one thing: IT DOESN'T FUCKING MATTER. _CHOOOOOOSE_
        //but, before i CHOOOOOOSE, figure out the fucking design of the merging! gah! brain is exploding over and over and over. there are so many places that a connection can go wrong, and i have to think of the case that accounts for all of them? or code for each separately? i can't even think about it properly. i don't know what the fuck to do. Wouldn't it involve the contents of clientshelper seeing as that's who stores the pending messages? am i going to make clientshelper an instance-per-connection type class as an optimization for accessing the m_Pending* lists, so that each connection has it's own list (along with each message type having it's own)???

        //TODOoptional: as a security measure, we could attempt to ask the OLD iodevice with the matching cookie if it is still there and if it is ok to be overwritten/whatever. if it responds at all, we pretty much want to NOT merge. if it is even able to respond, we should not be merging to begin with? i'd probably need a hard timeout for that asking


        //maybe this will become clear to me once i have the pending shit coded and working???? hmmm, since it depends on it, that actually sounds very likely
    }
    //else: dgaf, continue hello'ing as usual. maybe we deleted their queue already? their cookie doesn't mean shit to us, but we'll still use it... fuck it
}
void AbstractClientConnection::handleDataReceivedFromClient()
{
    emit d("got data from client in pre hello phase");

    while(!m_DataStreamToClient.atEnd())
    {
        if(!m_NetworkMagic.consumeFromIODeviceByteByByteLookingForMagic_And_ReturnTrueIf__Seen_or_PreviouslySeen__And_FalseIf_RanOutOfDataBeforeSeeingMagic(newClient))
        {
            return; //better luck next time. there is no need to 'continue;' because the function itself has it's own loop doing that
        }

        emit d("got passed magic check");

        //we get here:
        //a) if we read in all of magic just above
        //b) we read in magic previously but didn't have enough to read in the message
        //c) same as (b), but we didn't even have enough bytes to read in the message SIZE (i guess?)

        //read message size, message body, use it or whatever


        if(!m_IODevicePeeker->enoughBytesAreAvailableToReadTheByteArrayMessage())
        {
            return;
        }


        emit d("got enough data to read the message");
        //if we get here, we got enough to read in the qbytearray (including it's size, which we peeked :-P)
        //QByteArray tehFukkenMezzage;
        m_TheMessageByteArray.clear(); //TODOreq: is this necessary or does "streaming into" it overwrite previous contents?
        m_DataStreamToClient >> m_TheMessageByteArray; //woo. consume the message. we _know_ we have enough bytes!
        m_TheMessageBuffer.seek(0);
        m_NetworkMagic.messageHasBeenConsumedSoPlzResetMagic();

        if(!m_TheMessageByteArray.isNull() && !m_TheMessageByteArray.isEmpty())
        {
            //if it's null or empty then we just consume it (already done) and move on to the next message (including getting magic again)


            ServerHelloState currentState = m_ServerHelloState; //it's a copy because we change what we are switching from within the switch, which i *THINK* leads to disaster.. but could be wrong i forget and don't care to test it
            switch(currentState)
            {
                //TODOoptional: just as I have a magic to mark the start of each message, I could also have a "string verification" QString that is streamed in at the end (after the regular message params). it can be as simple as "asdf" and would be a lot easier to check than magic, since size is already checked etc. I'd just need to make sure to clear the string I am reading into each time, then after reading in the full message, check that the 'verification string' is in fact 'asdf'. It's I guess just another layer of protection against corruption and whatnot, idfk
                //^^i can use this in regular rpc generator messages as well and hide the checking from the rpc protocol impl entirely, idfk. perhaps it is overkill. i like overkill (but i don't like WASTE. xml is waste. also that kdab soap shit didn't mention anything about broadcasts (lol comet? laaame) or even message retrying etc. i like my solution))

                //this parser is so fucking ugly compared to my well-formed rpc generator's, it makes me sick.
                case DoneHelloing: //optimize for DoneHelloing case, as it will be used the most times over and over and over and over and over during production
                {
                    emit d("got a regular message during production mode (done helloing)");
                    m_ServerBusiness->messageReceived(&m_TheMessageByteArray, cookie());
                }
                break;
                case AwaitingHello:
                {
                    //read hello
                    emit d("got hello from client");
                    bool containsCookie = false;
                    m_TheMessageDataStream >> containsCookie; //still no idea if this takes 1 bit or 1 byte on the stream, but since it's inside a bytearray and we're measuring the size of THAT, it doesn't matter :-D. i like to think/hope/imagine that QDataStream would do a bool + quint32 (cookie) as 33 bits of network data.... but no clue tbh. also when we store it in a qbytearray it very likely rounds up to the nearest byte regardless lmfao... but i mean there are lots of other scenarios where i'd like to think bools are compacted into a single byte (if less than or equal to 8 bools in a row TODOreq: protocol design should make bools in a row if required, rpc generator can do this transparently after parsing the input xml. no fucking clue if i need to... or if bools are even stored as bits or bytes or what, lol)
                    if(containsCookie)
                    {
                        emit d("hello contains cookie, so this is a reconnect i guess");
                        quint32 cookieFromClient;
                        m_TheMessageDataStream >> cookieFromClient;

                        //TODOreq: eh i guess at some point (later on, but still has to do with hello protocol) i should trigger the sending of all their queued messages...
                        //TODOreq: I need to have a timeout for queued messages, how long I hold onto them. It'd probably only be a matter of seconds... minutes tops. It's to handle the disconnect/reconnect case. perhaps this shit is way teh fuck too robust... but you know what, over over under. fuck you tcp.

                        setCookie(cookieFromClient);

                        potentialMergeCaseAsCookieIsSupplied();
                    }

                    QByteArray welcomeMessage; //if i change the structure of this welcome message, i need to change the code in the client for the 'thank you', which just re-sends the welcome message (it is just a hack since they are identical for now)
                    QDataStream messageWriteStream(&welcomeMessage, QIODevice::WriteOnly);
                    messageWriteStream << serverHelloStatus->cookie(); //generates OR confirms cookie

                    NetworkMagic::streamOutMagic(&m_DataStreamToClient);
                    m_DataStreamToClient << welcomeMessage;
                    m_ServerHelloState = HelloReceivedWelcomeDispatchedAwaitingThankYouForWelcomingMe; //god this is so ugly. If only I had a way to easily generate message interactions cleanly based on some defined protocol... hmmm.....
                }
                break;
                case HelloReceivedWelcomeDispatchedAwaitingThankYouForWelcomingMe:
                {
                    emit d("got thank you for welcoming me");
                    //read thank you for welcoming me
                    quint32 cookieForConfirming;
                    m_TheMessageDataStream >> cookieForConfirming;
                    if(cookieForConfirming == serverHelloStatus->cookie())
                    {
                        emit d("the thank you for welcoming me cookie matched our previous cookie (whether we generated it or the client supplied it)");

                        //Now that it's done with the hello phase (more or less, we just have to give the client permission to start sending messages bro, which we're about to do shortly), we remove it from our "in hello phase" list and add it to our list of "current" ones that we can look up by clientId. i guess this means i should also GIVE it a clientId? are the cookie and clientId the same :-/?
                        //m_ServerHelloStatusesByIODevice.remove(newClient);

                        //Now add it to our active connections list. MIGHT AS WELL make clientId and cookie the same for simplicity. We are overwriting old/stale connections/qiodevices at this point. our parent/business maintains a list of clientIds by qiodevice, so HE can have multiple io devices all pointing to the same clientId (since the new/old iodevice is his key), but we only ever want to have one active connection (the most recent). by allowing the business to have multiple like that we just smooth the transition period between disconnect/instant-reconnect. it is very unlikely but still possible that we'd receive messages from the old qiodevice (TODOreq: when should i delete them? is that necessarily true? should i allow it to be true? does it mean i might get the same message twice? should i make it false so that is never the case? mindfuck)
                        //m_ClientsByCookie.insert(serverHelloStatus->cookie(), newClient);
                        m_ListOfHelloedConnections.append(this);

                        //TODOreq: maybe we should emit that the connection is good to go right here so that we GUARANTEE that we won't receive any of the messages (as a response to 'ok start sending messages bro') because we haven't even sent 'ok start sending messages bro' until we've rigged ourselves to listen to readyRead. we don't need readyRead anymore anyways :-P
                        //TODOoptional: i wonder if _WE_ should be responsible for doing the disconnect() to the readyRead instead of the listener of the following emit? it makes more sense i suppose...
                        //disconnect anyone who is listening to readyRead (the helloer in this case)

                        /*disconnect(newClient, SIGNAL(readyRead()), 0, 0);
                        emit connectionHasBeenHelloedAndIsReadyForAction(newClient, serverHelloStatus->cookie());*/

                        QByteArray okStartSendingMessagesBroMessage;
                        QDataStream messageWriteStream(&okStartSendingMessagesBroMessage, QIODevice::WriteOnly);
                        messageWriteStream << cookie();
                        //delete serverHelloStatus; //we don't need it anymore (right?????)
                        //messageWriteStream << QString("ok bro you're good to go"); //lol what else should i send? was thinking of just a bool. it makes no difference, hello doesn't need to be optimized lawl. i mean i guess there should just be an enum/int that identifies which message type it is you know? fuck it, the cookie, although redundant as fuck at this point, will serve as the "ok bro you're good to go"
                        m_ServerHelloState = DoneHelloing;
                        NetworkMagic::streamOutMagic(&m_DataStreamToClient);
                        m_DataStreamToClient << okStartSendingMessagesBroMessage; //TODOreq: i am forgetting magic for the client in my responses!!!
                    }
                    else
                    {
                        //TODOreq: confirm cookie failed, error out or something? dc? try again? send a HelloFailed message back? idfk
                    }
                }
                break;
                case ServerHelloStatus::ThankYouForWelcomingMeReceivedOkStartSendingBroDispatched: //because we shouldn't get here after we're done with the hello phase. this would be an error that we forgot to disconnect or whatever. in fact, we never even set our enum to this so it shouldn't even exist. we delete the serverHelloStatus because we're done with it.
                case ServerHelloStatus::HelloFailed:
                default:
                {
                    //TODOreq: handle errors n shit. dc?
                }
                break;
            }

        }
    }
}
