#include "abstractclientconnection.h"

#include "multiserverabstraction.h"
#include "iprotocolknower.h"
#include "iprotocolknowerfactory.h"

MultiServerAbstraction *AbstractClientConnection::m_MultiServerAbstraction = 0;
IProtocolKnowerFactory *AbstractClientConnection::m_ProtocolKnowerFactory = 0;

AbstractClientConnection::AbstractClientConnection(QIODevice *ioDeviceToClient, QObject *parent)
    : QObject(parent), m_IoDeviceToClient(ioDeviceToClient), m_DataStreamToClient(ioDeviceToClient), m_ServerHelloState(AwaitingHello), m_NetworkMagic(ioDeviceToClient), m_IODevicePeeker(ioDeviceToClient), m_HasCookie(false), m_MergeInProgress(false), m_OldConnectionToMergeOnto(0), m_ConnectionGood(false), m_ConnectionType(UnknownConnectionType)
{
    m_ReceivedMessageBuffer.setBuffer(&m_ReceivedMessageByteArray);
    m_ReceivedMessageBuffer.open(QIODevice::ReadWrite);
    m_ReceivedMessageDataStream.setDevice(&m_ReceivedMessageBuffer);

    //TODOreq: not a huge req, but if this constructor is called before calling setProtocolKnowerFactory, we'll segfault because it m_ProtocolKnowerFactory will be null. could just check it here, but idfk what to do if it's zero. should probably check from somewhere in caller, but also don't want to make that a task for the user to do. should still be somewhere in autogenerated code (so here if NEED BE)
    //^^^since it's called from the constructor of MultiServerAbstraction, it's probably SAFE ENOUGH. We could check for it not being null there (and only once (perfect)) though... :-)
    if(m_ProtocolKnowerFactory)
    {
        m_ProtocolKnower = m_ProtocolKnowerFactory->getNewProtocolKnower();
        m_ProtocolKnower->setMessageReceivedDataStream(&m_ReceivedMessageDataStream);
        m_ProtocolKnower->setAbstractClientConnection(this);
    }
    else
    {
        //TODOreq: forgot to set it statically before constructing!
    }

    if(!m_MultiServerAbstraction)
    {
        //TODOreq: forgot to set it statically before constructing!
    }

    setupConnectionsToIODevice();
    connect(this, SIGNAL(d(QString)), m_MultiServerAbstraction, SIGNAL(d(QString)));
}
AbstractClientConnection::~AbstractClientConnection()
{
    m_MultiServerAbstraction->reportConnectionDestroying(this);
    delete m_ProtocolKnower;
}
quint32 AbstractClientConnection::cookie()
{
    if(!m_HasCookie)
    {
        m_Cookie = m_MultiServerAbstraction->generateUnusedCookie(); /* after inventing the universe of course, that's implied */
        m_HasCookie = true;
    }
    return m_Cookie;
}
void AbstractClientConnection::setMergeInProgress(bool mergeInProgress)
{
    if(mergeInProgress != m_MergeInProgress)
    {
        //set up flagging for
         m_MergeInProgress = mergeInProgress;

         if(mergeInProgress)
         {
            //build a hash of all the ones in business pending RIGHT THIS VERY MOMENT because those ones are difficult to ensure they go back only when appropriate. specifically, if they are in business pending for the entire duration of the merge, they won't notice anything happened if they only depend on the flag above. this list ensures us that all the ones in business pending will wait for a retry
             //TODOreqNOWMOTHERFUCKER: determine if you want to use retry (message contents included) or a status ability. should the message retry also be a status poll if. ok fuck merging toggleBit with retryBit or anything with anything. if I'm struggling with it, I'm not KISS. so just use the flags for their names and yea that is the decision. if you add status (like a special-case-response) capabilities to the first retry (and probably the second too), that is fine i guess?

             //ok protocol knower is responsible for building the list. he is autogenerated code anyways so who gives a fuck. at a glance you'd think he wouldn't be suitable since he is an inheritence... but that doesn't matter in this case
             m_ProtocolKnower->notifyOfMergeInProgress();
         }
    }
}
void AbstractClientConnection::mergeNewIoDevice(QIODevice *newIoDeviceToClient)
{
    //definitely need to overwrite the io device, but might also need to flush the pending ack list? OR am I going with the design that says the client re-requests them?
    //also need to re-enable broadcasts
    //TODOreq: decide what to do with ones that were in business pending during the merge and whose request now correlates with the old connection. if we are strict, we should make them wait for the client to re-request them again. this is a hard to detect bug because queued mode is disabled when they are received from business pending (which is the NORMAL case, except this time it isn't normal because it happened over/during a merge)

    //schedule old io device for deletion (TODOreq: what about disconnection? are we guaranteed to already be disconnected if we get here? does a delete imply a disconnect?)
    m_IoDeviceToClient->close();
    m_IoDeviceToClient->deleteLater();

    //overwrite actual io device pointer
    m_IoDeviceToClient = newIoDeviceToClient;

    //connect to new io device
    setupConnectionsToIODevice();

    //connect to socket-specific error signals. let multiserverabstraction do it, because he does it initially and is better suited to knowing socket-specific functionality
    m_MultiServerAbstraction->setupSocketSpecificDisconnectAndErrorSignaling(m_IoDeviceToClient, this);

    //set io device as underlying io device for a few helper objects :-P
    m_DataStreamToClient.setDevice(newIoDeviceToClient);
    m_NetworkMagic.setIoDeviceToLookForMagicOn(newIoDeviceToClient);
    m_IODevicePeeker.setIoDeviceToPeek(newIoDeviceToClient);

    //add us to our done hello'ing list, which means we can be selected in broadcasts round robin selection
    m_MultiServerAbstraction->connectionDoneHelloing(this);


    //TODOreq: here would be a good place to put the turn off queue mode code, but i need to make sure to remember that there is a "race condition" where a message is in business pending and never gets queued (or more importantly, re-requested!) because it never runs into code that we own during the entire duration of the merge. it might be a rare race condition, but it could definitely happen <-- being handled
    setMergeInProgress(false); //TODOreq:^^^^
}
void AbstractClientConnection::setupConnectionsToIODevice()
{
    connect(m_IoDeviceToClient, SIGNAL(readyRead()), this, SLOT(handleDataReceivedFromClient()));
    //connect(this, SIGNAL(destroyed()), this, SLOT(deleteProtocolKnower())); //hack to delete the 'old' protocol newer when a connection merges. In our connection merge code, we call AbstractClientConnection->deleteLater(), but we can't call it on protocol knower since he isn't a qobject
    //^^nvm destroyed is emitted just before destructor, but destructor does the same thing! hack not needed
}
void AbstractClientConnection::handleDataReceivedFromClient()
{
    emit d("got data from client");

    while(!m_DataStreamToClient.atEnd())
    {
        if(!m_NetworkMagic.consumeFromIODeviceByteByByteLookingForMagic_And_ReturnTrueIf__Seen_or_PreviouslySeen__And_FalseIf_RanOutOfDataBeforeSeeingMagic())
        {
            return; //better luck next time. there is no need to 'continue;' because the function itself has it's own loop doing that
        }

        emit d("got passed magic check");

        //we get here:
        //a) if we read in all of magic just above
        //b) we read in magic previously but didn't have enough to read in the message
        //c) same as (b), but we didn't even have enough bytes to read in the message SIZE (i guess?)

        //read message size, message body, use it or whatever


        if(!m_IODevicePeeker.enoughBytesAreAvailableToReadTheByteArrayMessage())
        {
            return;
        }


        emit d("got enough data to read the message");
        //if we get here, we got enough to read in the qbytearray (including it's size, which we peeked :-P)
        //QByteArray tehFukkenMezzage;
        m_ReceivedMessageByteArray.clear(); //TODOreq: is this necessary or does "streaming into" it overwrite previous contents?
        m_DataStreamToClient >> m_ReceivedMessageByteArray; //woo. consume the message. we _know_ we have enough bytes!
        m_ReceivedMessageBuffer.seek(0);
        m_NetworkMagic.messageHasBeenConsumedSoPlzResetMagic();

        if(!m_ReceivedMessageByteArray.isNull() && !m_ReceivedMessageByteArray.isEmpty())
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
                    m_ProtocolKnower->messageReceived(); //&m_ReceivedMessageByteArray /*, cookie()*/);
                }
                break;
                case AwaitingHello:
                {
                    //read hello
                    emit d("got hello from client");
                    bool containsCookie = false;
                    m_ReceivedMessageDataStream >> containsCookie; //still no idea if this takes 1 bit or 1 byte on the stream, but since it's inside a bytearray and we're measuring the size of THAT, it doesn't matter :-D. i like to think/hope/imagine that QDataStream would do a bool + quint32 (cookie) as 33 bits of network data.... but no clue tbh. also when we store it in a qbytearray it very likely rounds up to the nearest byte regardless lmfao... but i mean there are lots of other scenarios where i'd like to think bools are compacted into a single byte (if less than or equal to 8 bools in a row TODOreq: protocol design should make bools in a row if required, rpc generator can do this transparently after parsing the input xml. no fucking clue if i need to... or if bools are even stored as bits or bytes or what, lol)

                    m_OldConnectionToMergeOnto = 0;

                    if(containsCookie)
                    {
                        emit d("hello contains cookie, so this is a reconnect i guess");
                        quint32 cookieFromClient;
                        m_ReceivedMessageDataStream >> cookieFromClient;

                        //TODOreq: eh i guess at some point (later on, but still has to do with hello protocol) i should trigger the sending of all their queued messages...
                        //TODOreq: I need to have a timeout for queued messages, how long I hold onto them. It'd probably only be a matter of seconds... minutes tops. It's to handle the disconnect/reconnect case. perhaps this shit is way teh fuck too robust... but you know what, over over under. fuck you tcp.

                        setCookie(cookieFromClient);

                        //detect if it's a merge, but don't actually do it yet
                        m_OldConnectionToMergeOnto = m_MultiServerAbstraction->potentialMergeCaseAsCookieIsSupplied_returning_oldConnection_ifMerge_or_ZERO_otherwise(this);
                    }

                    QByteArray welcomeMessage; //if i change the structure of this welcome message, i need to change the code in the client for the 'thank you', which just re-sends the welcome message (it is just a hack since they are identical for now)
                    QDataStream messageWriteStream(&welcomeMessage, QIODevice::WriteOnly);
                    messageWriteStream << cookie(); //generates OR confirms cookie

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
                    m_ReceivedMessageDataStream >> cookieForConfirming;
                    if(cookieForConfirming == cookie())
                    {
                        emit d("the thank you for welcoming me cookie matched our previous cookie (whether we generated it or the client supplied it)");

                        //Now that it's done with the hello phase (more or less, we just have to give the client permission to start sending messages bro, which we're about to do shortly), we remove it from our "in hello phase" list and add it to our list of "current" ones that we can look up by clientId. i guess this means i should also GIVE it a clientId? are the cookie and clientId the same :-/?
                        //m_ServerHelloStatusesByIODevice.remove(newClient);

                        //Now add it to our active connections list. MIGHT AS WELL make clientId and cookie the same for simplicity. We are overwriting old/stale connections/qiodevices at this point. our parent/business maintains a list of clientIds by qiodevice, so HE can have multiple io devices all pointing to the same clientId (since the new/old iodevice is his key), but we only ever want to have one active connection (the most recent). by allowing the business to have multiple like that we just smooth the transition period between disconnect/instant-reconnect. it is very unlikely but still possible that we'd receive messages from the old qiodevice (TODOreq: when should i delete them? is that necessarily true? should i allow it to be true? does it mean i might get the same message twice? should i make it false so that is never the case? mindfuck)
                        //m_ClientsByCookie.insert(serverHelloStatus->cookie(), newClient);
                        //m_ListOfHelloedConnections.append(this);

                        //do the merge if there's one to do
                        if(m_OldConnectionToMergeOnto)
                        {
                            //m_OldConnectionToMergeOnto->MERGE_SOMEHOW shit my brain just exploded TODO LEFT OFF
                            //the problem is that I have 2 abstractClientConnections and 2 protocolKnowers and 2 ioDevices, when I only want one of each. But I want to take/grab all of the "pending ack" and also receive "business pending" (TODOreq) that complete soon and send them over the NEW io device. the old io device is in queue mode at this moment so it doesn't even attempt to send, just adds to the pending ack list
                            //i think it would be simpler to delete the new 'abstractclientconnection' and 'protocol knower' and (after safely un-childing) moving the iodevice to to the old pair. the old protocol knower already has signals in the business pointed at it's slots, so it's definitely easier that way. TODOreq: might need to explicitly delete the iodevice if i DON'T have it as a child of [whichever of the pair own it, i forget]. Sweet just checked, the io device doesn't have a parent so moving it around is easy. That 'this' and/or 'protocolknower' are parented is irrelevant, can just deleteLater them once the io device is moved.

                            //disconnect all previous connections, as they are to the new abstract client connection, which will be deleted shortly (but we don't want to wait until then, we explicitly right now tell it to not handle the io device anymore). it is also true that readyRead should definitely not be lit up until a response to the ok start sending bro, which sends a few lines BELOW here... so it's a pretty strong guarantee that we don't have a race condition
                            disconnect(m_IoDeviceToClient, 0, 0, 0);

                            //the new connections from the new io device to the old connection that we're merging onto are setup in this call (also other stuff)
                            m_OldConnectionToMergeOnto->mergeNewIoDevice(m_IoDeviceToClient);

                            //disconnect(m_IoDeviceToClient, SIGNAL(readyRead()), this, SLOT(handleDataReceivedFromClient())); //TO Donereq: we might need to set a member bool to false also just in case there is a race condition where readRead signals have already been emitted??? if we do use the bool method then we don't even explicitly need this disconnect() call, as the deleteLater will handle the disconnections at the disappearance ofthe qobject. maybe if we do get a readyRead when that bool is true, we just call a public readyRead method on m_OldConnectionToMergeOnto (we're post-merge but pre-delete, so that pointer should still be valid). the public readyRead (or even any private ones) should be able to easily handle the case where they are called and they shouldn't have. the above solution might lead to it being called more frequently than it should. but i'd rather have it called more than it should than to miss calls!
                            //^^we are in pre HelloDone and client knows not to send until okBroStartSending which hasn't even been sent yet, so this disconnect call is perfectly ok. there will not be a readyRead until okStartSendingBro has been received/processed by the client. our own private impl code guarantees that. it is the implicit association (i should combine them into a namespace TODOreq/but also optional because i can always do this later when i NEED to do it (psbly famous last words i am unsure?))

                            QMetaObject::invokeMethod(this, "deleteLater", Qt::QueuedConnection); //we don't need the 'new' abstractClientConnections or the 'new' iprotocolknower (which gets deleted in 'this' destructor), and the 'new' iodevice has already been merged
                            //it is done queued because we want to ensure that the below okStartSendingBro definitely works
                        }
                        else
                        {
                            //new connection, not merging
                            m_MultiServerAbstraction->connectionDoneHelloing(this); //we don't want to activate this 'new' abstractClientConnection as being able to receive broadcasts because it doesn't have all the signals/slots from business already set up etc so fuck it. business pendings need to go to the old io device, and we aren't it
                        }

                        //TODOreq: now 'this' won't even exist later, so do we even want to continue on this path of execution since deleteLater has been called if we did a merge above? We essentially need to perform the rest of the code path on the old abstract client connection (or at least make that a part of mergeNewIoDevice method). the okStartSendingMessagesBro needs to still be sent, so we might want to continue ONLY this unit of execution and allow deleteLater to do it's stuff... later (guaranteed to be after okStartSendingBro below). BUT, the connectionDoneHelloing(this) bit should definitely change and/or be evaded (simple else { } just above would do). Since the okStartSendingBro deals only with the iodevice, it is ok to let the toBeDeletedLater 'this' to perform it's execution right here and how. TODOreq: in the above mergeNewIoDevice, call connectionDoneHelloing i guess? TODOreq: I might need to disconnect signals/slots from the iodevice to 'this' (which will be deleted later) because otherwise we might make it past this unit of execution when we definitely don't want to?

                        //we don't need to move the okStartSendingBroMessage from below, because it needs to be sent in both the merge and the new connection cases. it is a BIT hacky, but since it is only dependent on the [new] io device, we'll let it go

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
                case ThankYouForWelcomingMeReceivedOkStartSendingBroDispatched: //because we shouldn't get here after we're done with the hello phase. this would be an error that we forgot to disconnect or whatever. in fact, we never even set our enum to this so it shouldn't even exist. we delete the serverHelloStatus because we're done with it.
                case HelloFailed:
                default:
                {
                    //TODOreq: handle errors n shit. dc?
                }
                break;
            }

        }
    }
}
void AbstractClientConnection::makeConnectionBad()
{
    if(m_ConnectionGood)
    {
        m_ConnectionGood = false;

        //TODOreq: perhaps it should be done right here that we set the connection into a sort of merge mode. this is one of the detection points for it (pretty much ANY socket/network related error/disconnection whenever we aren't ready for a clean disconnect). We definitely want the connection to be marked as bad so that the messages are never sent, but it might be duplicated work because i think setting it to merge mode already does that? unsure tbh.
        //^^it might be as simple as setMergeInProgress(true) or whatever and getting rid of the m_ConnectionGood shit (though i should still have the check to make sure shit doesn't happen twice! we only react to the first error seen (though all are emit d()'d))
        //^^merge mode doesn't really factor in at all when the socket is entering the "connecting" state, for example... and tbh we don't know..... ahh shit lost my train of thought but now i'm thinking we should only do setMergeEnabled when we get here and connection is already good. if the connection was never good to begin with (such as the first "connecting" state transition), there's no need or point to go into merge mode

        //TODOreq: idfk wtf to do with anything that gets to the actual sending part and the connection is bad. do i just drop it or shouldn't i queue that shit? I _THINK_ since i'm coding all of this server shit on a single thread that proper checking ensures me it won't get there? I really don't know but worry it could maybe still get there because of race conditions. Doesn't the Qt::DirectConnection (implied) - 'ness of the socket erros mean that ....... ok nvm because the errors themselves aren't processed until we go back to the event loop, so we won't be in the middle of something and therefore won't be in the middle of dispatch to where we'd get a race condition.
        //^^There already is a race condition where an error is received but the event loop hasn't processed it, and that one is quite frankly impossible and I _THINK_ the rpc generator itself and it's ACK'ing design etc etc solve it? I think I'm just unable to see/remember how atm because I'm too sucked in. Like I think the merge code takes care of that??? Idfk. //TODOreq: think of cases where we'd be using the connection and whether it's good or bad based on actions (requests and responses) and broadcasts. It might be a waste of time and the design might account for it already, but I want to know that it does!
    }
}
void AbstractClientConnection::makeConnectionBadIfNewQAbstractSocketStateSucks(QAbstractSocket::SocketState newState)
{
    if(newState != QAbstractSocket::ConnectedState)
    {
        makeConnectionBad();
    }
    //you might think I should have an else here to make connection good, but that depends on more negotiations (ssl if using ssl, tcp has a negotiate too, and also my hello/cookie scheme in rpc generator (of which this is the primary use for now)
}
void AbstractClientConnection::makeConnectionBadIfNewQLocalSocketStateSucks(QLocalSocket::LocalSocketState newState)
{
    if(newState != QLocalSocket::ConnectedState)
    {
        makeConnectionBad();
    }
}