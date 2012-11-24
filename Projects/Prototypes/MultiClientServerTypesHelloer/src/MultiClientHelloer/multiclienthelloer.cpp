#include "multiclienthelloer.h"

MultiClientHelloer::MultiClientHelloer(QObject *parent) :
    QObject(parent)
{
}
MultiClientHelloer::~MultiClientHelloer()
{
    //TODOreq: iterate through all connections (pending and active) and delete them? disconnect first? no clue
}
void MultiClientHelloer::addSslClient()
{
    SslTcpClient *newSslClient = new SslTcpClient(this, ":/ClientCA.pem", ":/ServerCA.pem", ":/ClientPrivateKey.pem", ":/ClientPublicCert.pem", "fuckyou");

    connect(newSslClient, SIGNAL(connectedAndEncrypted(QSslSocket*)), this, SLOT(handleSslConnectionEstablished(QSslSocket*)));
    connect(newSslClient, SIGNAL(d(QString)), this, SIGNAL(d(QString)));


    m_ClientHelloStatusesByIODevice.insert(newSslClient, new ClientHelloStatus());

    if(!newSslClient->init())
    {
        emit d("failed to initialize new ssl client");
        return;
    }
    if(!newSslClient->start())
    {
        emit d("failed to start new ssl client");
        return;
    }
    emit d("new ssl client initialized and started");
}
//TODOoptimization: somehow we could use different objects for the slot we are connecting the readyRead signal to. i have barely thought this out and can't even really completely understand how it would work, but the idea is that a readyRead (applies to both hello phase and regular message phase) doesn't have to look up the clientId in a hash. the clientId becomes a member of the object that the slot lives on, and each connection now gets it's own object. i think it is an optimization because Qt already has to look up the object anyways... so this saves us from having to look up the clientId in a hash AS WELL. it might be slightly more memory intensive... but probably not even a relevant amount. PREMATURE OPTIMIZATION so fuck it for now, however
//in a sense i am / would be connecting readyRead to a slot on ClientHelloStatus (though it wouldn't be a struct any longer)
//in essence also ;-P LOL WUT
//it has nothing to do with multiple threads. i still don't understand how thread pools (per connection) work ;-P, QTcpSocket etc can't be used from multiple threads at the same time! bollocks! OH YOU MEAN THE 'PROCESSING' / 'BACKEND' THREADS? but then they must be synchronized if they access common data. fuck you. TODOreq: research if having multiple couchbase 'smart clients' on the same machine/process (but different threads) has a performance advantage. i'd imagine they can be in a different process, so they can PROBABLY be in different threads (would it be a 'smart client' per thread? OR the same thread-safe smart-client (think: pointer or object or whatever) used by all of those threads????? don't remember seeing anything about this on couchbase, so might need to ask on the forums etc :-)
void MultiClientHelloer::clientFinishedInitialConnectPhaseSoStartHelloing(QIODevice *newClient)
{
    ClientHelloStatus *status = m_ClientHelloStatusesByIODevice.value(newClient, 0);
    if(!status)
    {
        emit d("failed to find ClientHelloStatus for new connection");
        return;
    }

    emit d("initial connection complete");

    //dispatch hello
    QDataStream streamToServer(newClient);
    QByteArray helloMessage;
    QDataStream messageBuildingStream(&helloMessage, QIODevice::WriteOnly);
    messageBuildingStream << false; //TODOreq: test supplying the cookie in the hello (setting this to true + sending the cookie)
    NetworkMagic::streamOutMagic(&streamToServer);
    streamToServer << helloMessage;

    emit d("hello sent");

    status->m_ClientHelloState = ClientHelloStatus::HelloDispatchedAwaitingWelcome;
    connect(newClient, SIGNAL(readyRead()), this, SLOT(handleServerSentUsData()));

    delete status->m_IODevicePeeker;
    status->m_IODevicePeeker = new ByteArrayMessageSizePeekerForIODevice(newClient);
}
void MultiClientHelloer::handleServerSentUsData()
{
    emit d("got data from server");
    QIODevice *socketToServer = static_cast<QIODevice*>(sender());
    if(socketToServer)
    {
        ClientHelloStatus *clientHelloStatus = m_ClientHelloStatusesByIODevice.value(socketToServer, 0);
        if(clientHelloStatus)
        {
            QDataStream networkStreamToServer(socketToServer);

            while(!networkStreamToServer.atEnd())
            {
                if(!clientHelloStatus->m_NetworkMagic.consumeFromIODeviceByteByByteLookingForMagic_And_ReturnTrueIf__Seen_or_PreviouslySeen__And_FalseIf_RanOutOfDataBeforeSeeingMagic(socketToServer))
                {
                    return; //better luck next time. there is no need to 'continue;' because the function itself has it's own loop doing that
                }

                emit d("got passed magic check");

                if(!clientHelloStatus->m_IODevicePeeker->enoughBytesAreAvailableToReadTheByteArrayMessage())
                {
                    return;
                }

                emit d("got enough data to read the message");


                QByteArray tehFukkenMezzage;
                networkStreamToServer >> tehFukkenMezzage;
                clientHelloStatus->m_NetworkMagic.messageHasBeenConsumedSoPlzResetMagic();

                if(!tehFukkenMezzage.isNull() && !tehFukkenMezzage.isEmpty())
                {
                    QDataStream messageReadstream(&tehFukkenMezzage, QIODevice::ReadOnly);

                    ClientHelloStatus::ClientHelloState currentState = clientHelloStatus->m_ClientHelloState;
                    switch(currentState)
                    {
                        case ClientHelloStatus::HelloDispatchedAwaitingWelcome:
                        {
                            //read the welcome
                            quint32 cookie;
                            messageReadstream >> cookie;
                            //TODOreq: decide if we should compare it to our cookie that we sent (which is OPTIONAL, so we'd also have to remember if we even sent one) or if we should just allow the server to override the cookie regardless of if we sent one. probably the latter

                            //store the cookie the server assigned us (may or may not be what we already had)
                            clientHelloStatus->m_Cookie = cookie;

                            //dispatch the thank you
                            //hack: since we're just sending the cookie as the only message item, just use the same message we just read lol, fuck it
                            NetworkMagic::streamOutMagic(&networkStreamToServer);
                            networkStreamToServer << tehFukkenMezzage; //hack, should be a new message but oh well

                            clientHelloStatus->m_ClientHelloState = ClientHelloStatus::WelcomeReceivedThankYouForWelcomingMeSentAwaitingOkStartSendingBro;
                        }
                        break;
                        case ClientHelloStatus::WelcomeReceivedThankYouForWelcomingMeSentAwaitingOkStartSendingBro:
                        {
                            //read the ok start sending bro
                            quint32 cookie;
                            messageReadstream >> cookie;

                            if(clientHelloStatus->m_Cookie != cookie)
                            {
                                return; //TODOreq: handle this cookie confirmation better
                            }

                            emit d("received ok start sending bro woot");

                            m_ClientHelloStatusesByIODevice.remove(socketToServer);
                            m_ConnectionsByCookie.insert(cookie, socketToServer);
                            disconnect(socketToServer, SIGNAL(readyRead()), 0, 0);
                            emit connectionHasBeenHelloedAndIsReadyForAction(socketToServer, cookie);
                        }
                        break;
                        case ClientHelloStatus::HelloFailed:
                        default:
                            //TODOreq: handle errors n shit. dc?
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
void MultiClientHelloer::handleSslConnectionEstablished(QSslSocket *sslConnection)
{
    clientFinishedInitialConnectPhaseSoStartHelloing(sslConnection);
}
