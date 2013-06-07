#include "abstractserverconnection.h"

#include "multiserverclientabstraction.h"
#include "iprotocolknower.h"
#include "iprotocolknowerfactory.h"

MultiServerClientAbstraction *AbstractServerConnection::m_MultiServerClientAbstraction = 0;
IProtocolKnowerFactory *AbstractServerConnection::m_ProtocolKnowerFactory = 0;

AbstractServerConnection::AbstractServerConnection(QIODevice *ioDeviceToClient, QObject *parent)
    : QObject(parent), m_OldConnectionToMergeOnto(0), m_QueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection(false), m_IoDeviceToClient(ioDeviceToClient), m_DataStreamToServer(ioDeviceToClient), m_NetworkMagic(ioDeviceToClient),  m_IODevicePeeker(ioDeviceToClient), m_HasCookie(false)
{
    m_ReceivedMessageBuffer.setBuffer(&m_ReceivedMessageByteArray);
    m_ReceivedMessageBuffer.open(QIODevice::ReadWrite);
    m_ReceivedMessageDataStream.setDevice(&m_ReceivedMessageBuffer);

    if(m_ProtocolKnowerFactory)
    {
        m_ProtocolKnower = m_ProtocolKnowerFactory->getNewProtocolKnower();
        m_ProtocolKnower->setMessageReceivedDataStream(&m_ReceivedMessageDataStream);
        m_ProtocolKnower->setAbstractConnection(this);
    }
    else
    {
        //TODOreq: forgot to set it statically before constructing!
    }

    if(!m_MultiServerClientAbstraction)
    {
        //TODOreq: forgot to set it statically before constructing!
    }

    setupConnectionsToIODevice();
    connect(this, SIGNAL(d(QString)), m_MultiServerClientAbstraction, SIGNAL(d(QString)));



    //send initial hello. TODOreq: where will my entry point for reconnects be (re-send cookie)?

    //build byte array message
    QByteArray helloMessageByteArray;
    QDataStream helloMessageDataStream(&helloMessageByteArray, QIODevice::WriteOnly);
    helloMessageDataStream << (quint8)AbstractConnection::InitialHelloFromClient;
    helloMessageDataStream << false; //no cookie (if true, cookie follows)

    //stream magic header
    NetworkMagic::streamOutMagic(&m_DataStreamToServer);

    //stream message
    m_DataStreamToServer << helloMessageByteArray;
}
AbstractServerConnection::~AbstractServerConnection()
{
    m_MultiServerClientAbstraction->reportConnectionDestroying(this);
    delete m_ProtocolKnower;
}
quint32 AbstractServerConnection::cookie()
{
    if(!m_HasCookie)
    {
        //TODOreq: fix cookie logic because client does not generate one ever (though it can submit one when reconnecting). It'll probably be much simpler logic than what is on the server (and don't get confused by the fact that this is called AbstractServerConnection. It lives on the client)
        //m_Cookie = m_MultiServerClientAbstraction->generateUnusedCookie();
        m_HasCookie = true;
    }
    return m_Cookie;
}
void AbstractServerConnection::setQueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection(bool queueActionResponsesBecauseTheyMightBeReRequestedInNewConnection)
{
    if(queueActionResponsesBecauseTheyMightBeReRequestedInNewConnection != m_QueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection)
    {
         m_QueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection = queueActionResponsesBecauseTheyMightBeReRequestedInNewConnection;

         if(queueActionResponsesBecauseTheyMightBeReRequestedInNewConnection)
         {
             m_ProtocolKnower->queueActionResponsesHasBeenEnabledSoBuildLists();
         }
    }
}
void AbstractServerConnection::mergeNewIoDevice(QIODevice *newIoDeviceToClient)
{
    m_IoDeviceToClient->close();
    m_IoDeviceToClient->deleteLater();

    //overwrite actual io device pointer
    m_IoDeviceToClient = newIoDeviceToClient;

    //connect to new io device
    setupConnectionsToIODevice();

    //connect to socket-specific error signals. let multiserverabstraction do it, because he does it initially and is better suited to knowing socket-specific functionality
    m_MultiServerClientAbstraction->setupSocketSpecificDisconnectAndErrorSignaling(m_IoDeviceToClient, this);

    //set io device as underlying io device for a few helper objects :-P
    m_DataStreamToServer.setDevice(newIoDeviceToClient);
    m_NetworkMagic.setIoDeviceToLookForMagicOn(newIoDeviceToClient);
    m_IODevicePeeker.setIoDeviceToPeek(newIoDeviceToClient);

    //add us to our done hello'ing list, which means we can be selected in broadcasts round robin selection
    m_MultiServerClientAbstraction->connectionDoneHelloing(this);

    setQueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection(false);
}
void AbstractServerConnection::setupConnectionsToIODevice()
{
    connect(m_IoDeviceToClient, SIGNAL(readyRead()), this, SLOT(handleDataReceivedFromServer()));
}
void AbstractServerConnection::handleDataReceivedFromServer()
{
    emit d("got data from server");

    while(!m_DataStreamToServer.atEnd())
    {
        if(!m_NetworkMagic.consumeFromIODeviceByteByByteLookingForMagic_And_ReturnTrueIf__Seen_or_PreviouslySeen__And_FalseIf_RanOutOfDataBeforeSeeingMagic())
        {
            return;
        }

        emit d("got passed magic check");

        if(!m_IODevicePeeker.enoughBytesAreAvailableToReadTheByteArrayMessage())
        {
            return;
        }


        emit d("got enough data to read the message");

        m_ReceivedMessageByteArray.clear();
        m_DataStreamToServer >> m_ReceivedMessageByteArray;
        m_ReceivedMessageBuffer.seek(0);
        m_NetworkMagic.messageHasBeenConsumedSoPlzResetMagic();

        if(!m_ReceivedMessageByteArray.isNull() && !m_ReceivedMessageByteArray.isEmpty())
        {
            quint8 helloStateFromServerInMessage = 0x0;
            m_ReceivedMessageDataStream >> helloStateFromServerInMessage;

            switch(helloStateFromServerInMessage)
            {
                case DoneHelloingFromServer:
                {
                    emit d("got a regular message during production mode (done helloing)");
                    m_ProtocolKnower->messageReceived();
                }
                break;
                case WelcomeFromServer:
                {
                    //read welcome
                    emit d("got welcome from server with cookie: ");
                    quint32 cookie;
                    m_DataStreamToServer >> cookie;
                    setCookie(cookie);
                    emit d(QString::number(cookie));


                    //build thank you for welcoming me message
                    QByteArray thankYouForWelcomingMeByteArray;
                    QDataStream messageWriteStream(&thankYouForWelcomingMeByteArray, QIODevice::WriteOnly);
                    messageWriteStream << (quint8)AbstractConnection::ThankYouForWelcomingMeFromClient;
                    messageWriteStream << cookie();

                    //send thank you for welcoming me message
                    NetworkMagic::streamOutMagic(&m_DataStreamToServer);
                    m_DataStreamToServer << thankYouForWelcomingMeByteArray;
                }
                break;
                case OkStartSendingBroFromServer:
                {
                    //read ok start sending bro
                    quint32 cookie;
                    m_DataStreamToServer >> cookie;
                    if(cookie == cookie())
                    {
                        //TODOreq: how do i set myself up to be in DoneHelloing???? Do I just ignore this message? What the fuck is the point of it? How do I say. I'm going to use 'connectionDoneHelloing' which was copy/pasted from the server code, for now. At first glance it appears ok to call it, but what happens after that and the backing design etc have to be re-examined and maybe modified (or maybe it's fine, who knows (i used to and will again (hmm sounds like life)))
                        m_MultiServerClientAbstraction->connectionDoneHelloing(this);
                    }
                    else
                    {
                        //TODOreq: cookie with ok start sending bro (probably over-verified at this point and not likely to happen anyways) didn't match one seen/set/something earlier... so handle this error appropriately. still have the same problem in server (except that time, the verification is actually a good idea) so the two can probably be solved together
                    }
                }
                break;
                case InvalidHelloStateFromServer:
                default:
                {
                    //TODOreq: handle errors n shit. dc?
                }
                break;
            }
        }
    }
}
void AbstractServerConnection::makeConnectionBad()
{
        m_MultiServerClientAbstraction->dontBroadcastTo(this);

        m_MultiServerClientAbstraction->appendDeadConnectionThatMightBePickedUpLater(this);

        setQueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection(true);
}
void AbstractServerConnection::makeConnectionBadIfNewQAbstractSocketStateSucks(QAbstractSocket::SocketState newState)
{
    if(newState != QAbstractSocket::ConnectedState)
    {
        makeConnectionBad();
    }
}
void AbstractServerConnection::makeConnectionBadIfNewQLocalSocketStateSucks(QLocalSocket::LocalSocketState newState)
{
    if(newState != QLocalSocket::ConnectedState)
    {
        makeConnectionBad();
    }
}
