#include "multiserverhelloer.h"

MultiServerHelloer::MultiServerHelloer(QObject *parent) :
    QObject(parent), m_SslTcpServer(0)
{
}
void MultiServerHelloer::startAll3Listening()
{
    if(!m_SslTcpServer)
    {
        m_SslTcpServer = new SslTcpServer(this, ":/RpcBankServerCA.pem", ":/RpcBankServerClientCA.pem", ":/RpcBankServerPrivateKey.pem", ":/RpcBankServerPublicCert.pem", "fuckyou");
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
void MultiServerHelloer::handleNewClientConnected(QIODevice *newClient)
{
    m_ServerHelloStatusesByIODevice.insert(newClient, new ServerHelloStatus()); //TODOreq: delete ServerHelloStatus somewhere (dc? dupe detected? both? idfk)
    connect(newClient, SIGNAL(readyRead()), this, SLOT(handleNewClientSentData()));
}
void MultiServerHelloer::handleNewClientSentData()
{
    QIODevice *newClient = static_cast<QIODevice*>(sender());
    if(newClient)
    {
        ServerHelloStatus *serverHelloStatus = m_ServerHelloStatusesByIODevice.value(newClient, 0);
        if(serverHelloStatus)
        {
            QDataStream stream(newClient);

            while(!stream.atEnd())
            {

                //we read in all of the bytes available on at a time until we either run out of bytes (TODOreq: but we need to remember the state of magic and pick up when we get more bytes!) or we have seen 'magic', and then let it pass to the message processing part

                //this if statement is to skip reading in magic if we previously got it (so magic would be 'good'), but we didn't have enough data... but we don't want to or need to read in magic again. we don't "need" it. the magicIsGood() below can't be in an "else" because we still want to continue with the normal message processing if we happen to get magic in one go. TODOreq: there is a rare case where we have JUST enough to read in magic but not enough to read in the message. that's the purpose of the 'needsMagic' check
                if(serverHelloStatus->needsMagic())
                {
                    if(newClient->bytesAvailable() < 1)
                    {
                        return; //needs moar bytes
                    }

                    quint8 magicPiece;
                    stream >> magicPiece;

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
                qint64 bytesPeeked = newClient->peek(&peekedByteArraySize, sizeof(quint32)); //TODOreq: this might not work because we're not longer using QDataStream's encoding/decoding??? little endian etc etc, who the fuck knows
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

                //if we get here, we got enough to read in the qbytearray (including it's size, which we peeked :-P)
                QByteArray tehFukkenMezzage;
                stream >> tehFukkenMezzage; //woo
                serverHelloStatus->setMagicGot(false); //only after reading in a full message, set magicGot back to false

                if(!tehFukkenMezzage.isNull() && !tehFukkenMezzage.isEmpty())
                {
                    //if it's null or empty then we just consume it (already done) and move on to the next message (including getting magic again)

                    //regular message processing goes here :)
                    QDataStream stream2(&tehFukkenMezzage, QIODevice::ReadOnly);

                    ServerHelloStatus::ServerHelloState currentState = serverHelloStatus->m_ServerHelloState; //it's a copy because we change what we are switching from within the switch, which i *THINK* leads to disaster.. but could be wrong i forget and don't care to test it
                    switch(currentState)
                    {
                        //TODOoptional: just as I have a magic to mark the start of each message, I could also have a "string verification" QString that is streamed in at the end (after the regular message params). it can be as simple as "asdf" and would be a lot easier to check than magic, since size is already checked etc. I'd just need to make sure to clear the string I am reading into each time, then after reading in the full message, check that the 'verification string' is in fact 'asdf'. It's I guess just another layer of protection against corruption and whatnot, idfk
                        //^^i can use this in regular rpc generator messages as well and hide the checking from the rpc protocol impl entirely, idfk. perhaps it is overkill. i like overkill (but i don't like WASTE. xml is waste. also that kdab soap shit didn't mention anything about broadcasts (lol comet? laaame) or even message retrying etc. i like my solution))

                        //this parser is so fucking ugly compared to my well-formed rpc generator's, it makes me sick.

                        case ServerHelloStatus::AwaitingHello:
                        {
                            //read hello
                            bool containsCookie = false;
                            stream2 >> containsCookie; //still no idea if this takes 1 bit or 1 byte on the stream, but since it's inside a bytearray and we're measuring the size of THAT, it doesn't matter :-D
                            if(containsCookie)
                            {
                                QString cookieFromClient;
                                stream2 >> cookieFromClient;

                                //TODOreq: eh i guess at some point (later on, but still has to do with hello protocol) i should trigger the sending of all their queued messages...
                                //TODOreq: I need to have a timeout for queued messages, how long I hold onto them. It'd probably only be a matter of seconds... minutes tops. It's to handle the disconnect/reconnect case. perhaps this shit is way teh fuck too robust... but you know what, over over under. fuck you tcp.

                                serverHelloStatus->setCookie(cookieFromClient);
                            }

                            TODO lEFT OFF
                            QByteArray welcomeMessage; //TODOreq: magic for client ++++ we need to stream into this bytearray using a qdatastream noob, not append! way to fail
                            welcomeMessage.append(serverHelloStatus->cookie()); //generates OR confirms cookie
                            stream << welcomeMessage;
                            serverHelloStatus->m_ServerHelloState = ServerHelloStatus::HelloReceivedWelcomeDispatchedAwaitingThankYouForWelcomingMe; //god this is so ugly. If only I had a way to easily generate message interactions cleanly based on some defined protocol... hmmm.....
                        }
                        break;
                        case ServerHelloStatus::HelloReceivedWelcomeDispatchedAwaitingThankYouForWelcomingMe:
                        {
                            //read thank you for welcoming me
                            QString confirmedHello;
                            stream2 >> confirmedHello;
                            if(confirmedHello == serverHelloStatus->cookie())
                            {
                                //TODOreq: maybe we should emit that the connection is good to go right here so that we GUARANTEE that we won't receive any of the messages (as a response to 'ok start sending messages bro') because we haven't even sent 'ok start sending messages bro' until we've rigged ourselves to listen to readyRead. we don't need readyRead anymore anyways :-P
                                QByteArray okStartSendingMessagesBro;
                                okStartSendingMessagesBro << QString("ok bro you're good to go"); //lol what else should i send? was thinking of just a bool. it makes no difference, hello doesn't need to be optimized lawl
                                stream << okStartSendingMessagesBro; //TODOreq: i am forgetting magic for the client in my responses!!!
                            }
                            else
                            {
                                //TODOreq: confirm cookie failed, error out or something? dc? try again? send a HelloFailed message back? idfk
                            }
                        }
                        break;
                        case ServerHelloStatus::ThankYouForWelcomingMeReceivedOkStartSendingBroDispatched: //because we shouldn't get here after we're done with the hello phase. this would be an error that we forgot to disconnect or whatever...
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
        }
    }
    else
    {
        //invalid sender... should also never happen
    }
}
