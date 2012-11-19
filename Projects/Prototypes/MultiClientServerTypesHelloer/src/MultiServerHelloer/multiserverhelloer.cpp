#include "multiserverhelloer.h"

const quint8 ServerHelloStatus::m_MagicExpected[MAGIC_BYTE_SIZE] =  { 'F', 'U', 'C', 'K' };
quint32 ServerHelloStatus::overflowingClientIdsUsedAsInputForMd5er = 0;

MultiServerHelloer::MultiServerHelloer(QObject *parent) :
    QObject(parent), m_SslTcpServer(0)
{
}
MultiServerHelloer::~MultiServerHelloer()
{
    //TODOreq: not sure, but i think i need to iterate through all pending/active connections and delete them? disconnect first? no fucking clue
    delete m_SslTcpServer;
}
void MultiServerHelloer::startAll3Listening()
{
    emit d("trying to start all 3 listening");
    if(!m_SslTcpServer)
    {
        m_SslTcpServer = new SslTcpServer(this, ":/ServerCA.pem", ":/ClientCA.pem", ":/ServerPrivateKey.pem", ":/ServerPublicCert.pem", "fuckyou");
        connect(m_SslTcpServer, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
        connect(m_SslTcpServer, SIGNAL(clientConnectedAndEncrypted(QSslSocket*)), this, SLOT(handleClientConnectedAndEncrypted(QSslSocket*)));

        if(m_SslTcpServer->init())
        {
            emit d("ssl tcp server initialized");

            if(m_SslTcpServer->start())
            {
                emit d("ssl tcp server started");
            }
            else
            {
                emit d("ssl tcp server failed to start");
            }
        }
        else
        {
            emit d("ssl tcp server failed to initialize");
        }

    }
}
void MultiServerHelloer::streamOutMagic(QDataStream *ds)
{
    *ds << ServerHelloStatus::m_MagicExpected[0];
    *ds << ServerHelloStatus::m_MagicExpected[1];
    *ds << ServerHelloStatus::m_MagicExpected[2];
    *ds << ServerHelloStatus::m_MagicExpected[3];
}
void MultiServerHelloer::handleNewClientConnected(QIODevice *newClient)
{
    emit d("new client connected, starting hello status");
    m_ServerHelloStatusesByIODevice.insert(newClient, new ServerHelloStatus()); //TODOreq: delete ServerHelloStatus somewhere (dc? dupe detected? both? idfk)
    connect(newClient, SIGNAL(readyRead()), this, SLOT(handleNewClientSentData()));
}
void MultiServerHelloer::handleNewClientSentData()
{
    emit d("got data from client");
    QIODevice *newClient = static_cast<QIODevice*>(sender());
    if(newClient)
    {
        ServerHelloStatus *serverHelloStatus = m_ServerHelloStatusesByIODevice.value(newClient, 0);
        if(serverHelloStatus)
        {
            QDataStream networkStreamToClient(newClient);

            while(!networkStreamToClient.atEnd())
            {

                //we read in all of the bytes available on at a time until we either run out of bytes (TODOreq: but we need to remember the state of magic and pick up when we get more bytes!) or we have seen 'magic', and then let it pass to the message processing part

                //this if statement is to skip reading in magic if we previously got it (so magic would be 'good'), but we didn't have enough data... but we don't want to or need to read in magic again. we don't "need" it. the magicIsGood() below can't be in an "else" because we still want to continue with the normal message processing if we happen to get magic in one go. TODOreq: there is a rare case where we have JUST enough to read in magic but not enough to read in the message. that's the purpose of the 'needsMagic' check
                if(serverHelloStatus->needsMagic())
                {
                    emit d("need magic");
                    if(newClient->bytesAvailable() < 1)
                    {
                        return; //needs moar bytes
                    }

                    quint8 magicPiece;
                    networkStreamToClient >> magicPiece;

                    if(serverHelloStatus->m_MagicExpected[serverHelloStatus->m_CurrentMagicByteIndex] == magicPiece)
                    {
                        ++serverHelloStatus->m_CurrentMagicByteIndex;

                        if(serverHelloStatus->m_CurrentMagicByteIndex == MAGIC_BYTE_SIZE)
                        {
                            //done with magic
                            serverHelloStatus->setMagicGot(true);
                            serverHelloStatus->m_CurrentMagicByteIndex = 0; //for next time, though we could do this when doing setMagicGot(false) below (after reading in a full message)
                        }
                        else
                        {
                            continue; //needz moar magic, and we may or may not have more bytes
                        }
                    }
                    else
                    {
                        //we got an unexpected byte, so we restart the magic seeking process. this doesn't necessarily mean we are out of bytes
                        serverHelloStatus->m_CurrentMagicByteIndex = 0;
                    }
                }

                emit d("got passed magic check");


                //we get here:
                //a) if we read in all of magic just above
                //b) we read in magic previously but didn't have enough to read in the message
                //c) same as (b), but we didn't even have enough bytes to read in the message size (i guess?)

                //read message size, message body, use it or whatever


                if(newClient->bytesAvailable() < sizeof(quint32))
                {
                    return; //needs moar bytes
                }

                quint32 peekedByteArraySize;
                qint64 bytesPeeked = newClient->peek((char*)&peekedByteArraySize /* i just threw up, but i _THINK_ this will work? */, sizeof(quint32)); //TODOreq: this might not work because we're not longer using QDataStream's encoding/decoding??? little endian etc etc, who the fuck knows
                if(bytesPeeked != sizeof(quint32))
                {
                    return; //needs moar bytes (we should never get here beause we check bytesAvailable above)
                }

                if(peekedByteArraySize != 0xFFFFFFFF && peekedByteArraySize != 0x0) //we still want to read in the message if it's null or empty
                {
                    if((newClient->bytesAvailable()-sizeof(quint32)) < peekedByteArraySize) //we check to see if there's enough bytesAvailable that the byteArray requires
                    {
                        return; //needs moar bytes!
                    }
                }

                emit d("got enough data to read the message");
                //if we get here, we got enough to read in the qbytearray (including it's size, which we peeked :-P)
                QByteArray tehFukkenMezzage;
                networkStreamToClient >> tehFukkenMezzage; //woo
                serverHelloStatus->setMagicGot(false); //only after reading in a full message, set magicGot back to false

                if(!tehFukkenMezzage.isNull() && !tehFukkenMezzage.isEmpty())
                {
                    //if it's null or empty then we just consume it (already done) and move on to the next message (including getting magic again)

                    //regular message processing goes here :)
                    QDataStream messageReadstream(&tehFukkenMezzage, QIODevice::ReadOnly);

                    ServerHelloStatus::ServerHelloState currentState = serverHelloStatus->m_ServerHelloState; //it's a copy because we change what we are switching from within the switch, which i *THINK* leads to disaster.. but could be wrong i forget and don't care to test it
                    switch(currentState)
                    {
                        //TODOoptional: just as I have a magic to mark the start of each message, I could also have a "string verification" QString that is streamed in at the end (after the regular message params). it can be as simple as "asdf" and would be a lot easier to check than magic, since size is already checked etc. I'd just need to make sure to clear the string I am reading into each time, then after reading in the full message, check that the 'verification string' is in fact 'asdf'. It's I guess just another layer of protection against corruption and whatnot, idfk
                        //^^i can use this in regular rpc generator messages as well and hide the checking from the rpc protocol impl entirely, idfk. perhaps it is overkill. i like overkill (but i don't like WASTE. xml is waste. also that kdab soap shit didn't mention anything about broadcasts (lol comet? laaame) or even message retrying etc. i like my solution))

                        //this parser is so fucking ugly compared to my well-formed rpc generator's, it makes me sick.

                        case ServerHelloStatus::AwaitingHello:
                        {
                            //read hello
                            emit d("got hello from client");
                            bool containsCookie = false;
                            messageReadstream >> containsCookie; //still no idea if this takes 1 bit or 1 byte on the stream, but since it's inside a bytearray and we're measuring the size of THAT, it doesn't matter :-D
                            if(containsCookie)
                            {
                                emit d("hello contains cookie, so this is a reconnect i guess");
                                quint32 cookieFromClient;
                                messageReadstream >> cookieFromClient;

                                //TODOreq: eh i guess at some point (later on, but still has to do with hello protocol) i should trigger the sending of all their queued messages...
                                //TODOreq: I need to have a timeout for queued messages, how long I hold onto them. It'd probably only be a matter of seconds... minutes tops. It's to handle the disconnect/reconnect case. perhaps this shit is way teh fuck too robust... but you know what, over over under. fuck you tcp.

                                serverHelloStatus->setCookie(cookieFromClient);
                            }

                            QByteArray welcomeMessage; //TODOreq: magic for client ++++ we need to stream into this bytearray using a qdatastream noob, not append! way to fail
                            QDataStream messageWriteStream(&welcomeMessage, QIODevice::WriteOnly);
                            streamOutMagic(&messageWriteStream);
                            messageWriteStream << serverHelloStatus->cookie(); //generates OR confirms cookie
                            networkStreamToClient << welcomeMessage;
                            serverHelloStatus->m_ServerHelloState = ServerHelloStatus::HelloReceivedWelcomeDispatchedAwaitingThankYouForWelcomingMe; //god this is so ugly. If only I had a way to easily generate message interactions cleanly based on some defined protocol... hmmm.....
                        }
                        break;
                        case ServerHelloStatus::HelloReceivedWelcomeDispatchedAwaitingThankYouForWelcomingMe:
                        {
                            emit d("got thank you for welcoming me");
                            //read thank you for welcoming me
                            quint32 cookieForConfirming;
                            messageReadstream >> cookieForConfirming;
                            if(cookieForConfirming == serverHelloStatus->cookie())
                            {
                                emit d("the thank you for welcoming me cookie matched our previous cookie (whether we generated it or the client supplied it)");

                                //Now that it's done with the hello phase (more or less, we just have to give the client permission to start sending messages bro, which we're about to do shortly), we remove it from our "in hello phase" list and add it to our list of "current" ones that we can look up by clientId. i guess this means i should also GIVE it a clientId? are the cookie and clientId the same :-/?
                                m_ServerHelloStatusesByIODevice.remove(newClient);

                                //Now add it to our active connections list. MIGHT AS WELL make clientId and cookie the same for simplicity. We are overwriting old/stale connections/qiodevices at this point. our parent/business maintains a list of clientIds by qiodevice, so HE can have multiple io devices all pointing to the same clientId (since the new/old iodevice is his key), but we only ever want to have one active connection (the most recent). by allowing the business to have multiple like that we just smooth the transition period between disconnect/instant-reconnect. it is very unlikely but still possible that we'd receive messages from the old qiodevice (TODOreq: when should i delete them? is that necessarily true? should i allow it to be true? does it mean i might get the same message twice? should i make it false so that is never the case? mindfuck)
                                m_ClientsByCookie.insert(serverHelloStatus->cookie(), newClient);

                                //TODOreq: maybe we should emit that the connection is good to go right here so that we GUARANTEE that we won't receive any of the messages (as a response to 'ok start sending messages bro') because we haven't even sent 'ok start sending messages bro' until we've rigged ourselves to listen to readyRead. we don't need readyRead anymore anyways :-P
                                //TODOoptional: i wonder if _WE_ should be responsible for doing the disconnect() to the readyRead instead of the listener of the following emit? it makes more sense i suppose...
                                //disconnect anyone who is listening to readyRead (the helloer in this case)
                                disconnect(newClient, SIGNAL(readyRead()), 0, 0);
                                emit connectionHasBeenHelloedAndIsReadyForAction(newClient, serverHelloStatus->cookie());

                                QByteArray okStartSendingMessagesBroMessage;
                                QDataStream messageWriteStream(&okStartSendingMessagesBroMessage, QIODevice::WriteOnly);
                                streamOutMagic(&messageWriteStream);
                                messageWriteStream << serverHelloStatus->cookie();
                                delete serverHelloStatus; //we don't need it anymore (right?????)
                                messageWriteStream << QString("ok bro you're good to go"); //lol what else should i send? was thinking of just a bool. it makes no difference, hello doesn't need to be optimized lawl. i mean i guess there should just be an enum/int that identifies which message type it is you know?
                                networkStreamToClient << okStartSendingMessagesBroMessage; //TODOreq: i am forgetting magic for the client in my responses!!!
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
        else
        {
            //somehow received data from a client who skipped the handleNewClientConnected phase, wtf? should never happen. mb block the sender or fuck i have no idea what to do
            //we can also get here because our cookie gets removed/'deleted'. i mean, we still shouldn't get here... but let me just say that it's very unlikely to get to the 'default:' case in the switch. we're much more likely to get here :-P. STILL, they should both error out the same way... (which is????)
        }
    }
    else
    {
        //invalid sender... should also never happen
    }
}
