#include "abstractserverconnection.h"

#include "multiserverclientabstraction.h"
#include "iprotocolknower.h"
#include "iprotocolknowerfactory.h"

MultiServerClientAbstraction *AbstractServerConnection::m_MultiServerClientAbstraction = 0;
IProtocolKnowerFactory *AbstractServerConnection::m_ProtocolKnowerFactory = 0;

AbstractServerConnection::AbstractServerConnection(QIODevice *ioDeviceToClient, QObject *parent)
    : QObject(parent), m_OldConnectionToMergeOnto(0), m_QueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection(false), m_IoDeviceToClient(ioDeviceToClient), m_DataStreamToClient(ioDeviceToClient), m_NetworkMagic(ioDeviceToClient),  m_IODevicePeeker(ioDeviceToClient), m_HasCookie(false)
{
    m_ReceivedMessageBuffer.setBuffer(&m_ReceivedMessageByteArray);
    m_ReceivedMessageBuffer.open(QIODevice::ReadWrite);
    m_ReceivedMessageDataStream.setDevice(&m_ReceivedMessageBuffer);

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

    if(!m_MultiServerClientAbstraction)
    {
        //TODOreq: forgot to set it statically before constructing!
    }

    setupConnectionsToIODevice();
    connect(this, SIGNAL(d(QString)), m_MultiServerClientAbstraction, SIGNAL(d(QString)));
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
        m_Cookie = m_MultiServerClientAbstraction->generateUnusedCookie();
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
    m_DataStreamToClient.setDevice(newIoDeviceToClient);
    m_NetworkMagic.setIoDeviceToLookForMagicOn(newIoDeviceToClient);
    m_IODevicePeeker.setIoDeviceToPeek(newIoDeviceToClient);

    //add us to our done hello'ing list, which means we can be selected in broadcasts round robin selection
    m_MultiServerClientAbstraction->connectionDoneHelloing(this);

    setQueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection(false);
}
void AbstractServerConnection::setupConnectionsToIODevice()
{
    connect(m_IoDeviceToClient, SIGNAL(readyRead()), this, SLOT(handleDataReceivedFromClient()));
}
void AbstractServerConnection::handleDataReceivedFromClient()
{
    emit d("got data from client");

    while(!m_DataStreamToClient.atEnd())
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
        m_DataStreamToClient >> m_ReceivedMessageByteArray;
        m_ReceivedMessageBuffer.seek(0);
        m_NetworkMagic.messageHasBeenConsumedSoPlzResetMagic();

        if(!m_ReceivedMessageByteArray.isNull() && !m_ReceivedMessageByteArray.isEmpty())
        {
            quint8 helloStateFromClientInMessage = 0x0;
            m_ReceivedMessageDataStream >> helloStateFromClientInMessage;

            switch(helloStateFromClientInMessage)
            {
                case DoneHelloingFromClient:
                {
                    emit d("got a regular message during production mode (done helloing)");
                    m_ProtocolKnower->messageReceived();
                }
                break;
                //case AwaitingHello:
                case InitialHelloFromClient:
                {
                    //read hello
                    emit d("got hello from client");
                    bool containsCookie = false;
                    m_ReceivedMessageDataStream >> containsCookie;
                    m_OldConnectionToMergeOnto = 0;
                    if(containsCookie)
                    {
                        emit d("hello contains cookie, so this is a reconnect i guess");
                        quint32 cookieFromClient;
                        m_ReceivedMessageDataStream >> cookieFromClient;

                        setCookie(cookieFromClient);

                        m_OldConnectionToMergeOnto = m_MultiServerClientAbstraction->potentialMergeCaseAsCookieIsSupplied_returning_oldConnection_ifMerge_or_ZERO_otherwise(this);
                    }

                    QByteArray welcomeMessage;
                    QDataStream messageWriteStream(&welcomeMessage, QIODevice::WriteOnly);
                    messageWriteStream << cookie();

                    NetworkMagic::streamOutMagic(&m_DataStreamToClient);
                    m_DataStreamToClient << welcomeMessage;
                }
                break;
                case ThankYouForWelcomingMeFromClient:
                {
                    emit d("got thank you for welcoming me");
                    //read thank you for welcoming me
                    quint32 cookieForConfirming;
                    m_ReceivedMessageDataStream >> cookieForConfirming;
                    if(cookieForConfirming == cookie())
                    {
                        emit d("the thank you for welcoming me cookie matched our previous cookie (whether we generated it or the client supplied it)");

                        if(m_OldConnectionToMergeOnto)
                        {
                            disconnect(m_IoDeviceToClient, 0, 0, 0);

                            m_OldConnectionToMergeOnto->mergeNewIoDevice(m_IoDeviceToClient);

                            QMetaObject::invokeMethod(this, "deleteLater", Qt::QueuedConnection);
                        }
                        else
                        {
                            //new connection, not merging
                            m_MultiServerClientAbstraction->connectionDoneHelloing(this);
                        }

                        QByteArray okStartSendingMessagesBroMessage;
                        QDataStream messageWriteStream(&okStartSendingMessagesBroMessage, QIODevice::WriteOnly);
                        messageWriteStream << cookie();

                        NetworkMagic::streamOutMagic(&m_DataStreamToClient);
                        m_DataStreamToClient << okStartSendingMessagesBroMessage;
                    }
                    else
                    {

                    }
                }
                break;
                case InvalidHelloStateFromClient:
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
