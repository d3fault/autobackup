#include "testserver.h"

#include "simpleassprotocol.h"

TestServer::TestServer(QObject *parent) :
    QObject(parent)
{
    //m_SizeExpecting = sizeof(MessageHeader);
}
void TestServer::init()
{
    m_Server = new SslTcpServer(this, ":/ServerCA.pem", ":/ClientCA.pem", ":/ServerPrivateKey.pem", ":/ServerPublicCert.pem", "fuckyou");

    connect(m_Server, SIGNAL(clientConnectedAndEncrypted(QSslSocket*)), this, SLOT(handleClientConnectedAndEncrypted(QSslSocket*)));
    connect(m_Server, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    if(m_Server->init())
    {
        emit initialized();
        emit d("server initialized");
    }
    else
    {
        emit d("server failed to initialize");
    }
}
void TestServer::start()
{
    if(m_Server->start())
    {
        emit started();
        emit d("server started");
    }
    else
    {
        emit d("server failed to start");
    }
}
void TestServer::stop()
{
    m_Server->stop();
    emit stopped();
}
void TestServer::handleClientConnectedAndEncrypted(QSslSocket *clientSocket)
{
    m_ClientSocket = clientSocket;
    connect(m_ClientSocket, SIGNAL(readyRead()), this, SLOT(handleClientSentUsData()));
    emit d("client connected and encrypted");
}
void TestServer::handleClientSentUsData()
{
    //sender or m_ClientSocket... they are/should be/will be the same for this test

    QDataStream stream(m_ClientSocket);
    while(!stream.atEnd())
    {
        MyMessageHeader header;
        stream >> header;
        switch(header.MessageType)
        {
        case MyMessageHeader::MessageAType:
            {
                MyMessageABody body;
                stream >> body;
                emit d(QString("Type: A, " + QString(" message received from client:")));
                QString commaSpace(", ");
                emit d(QString::number(body.RandomInt1) + commaSpace + QString::number(body.RandomUInt2) + commaSpace + body.RandomString1 + commaSpace + body.RandomString2);
            }
        break;
        case MyMessageHeader::MessageBType:
            {
                MyMessageBBody body;
                stream >> body;
                emit d(QString("Type: B, " + QString(" message received from client:")));
                QString commaSpace(", ");
                emit d(QString::number(body.RandomInt1) + commaSpace + QString::number(body.RandomUInt2) + commaSpace + body.RandomString1 + commaSpace + body.RandomString2);
            }
        break;
        default:
            break;
        }
    }

#ifdef PROPOSED_SOLUTION
    while(!stream.atEnd())
    {
        qint64 bytesAvailableToStart = m_ClientSocket->bytesAvailable();
        if(bytesAvailableToStart >= m_SizeExpecting)
        {
            switch(m_IntStateKeeperTracker)
            {
            case 0:
            {
                //read in header, if headerSize is available
                if(bytesAvailableToStart >= sizeof(MessageHeader))
                {
                    //MessageHeader header;
                    stream >> *m_CurrentHeader;

                    quint64 bytesAvailableAfterReadingHeader = m_ClientSocket->bytesAvailable();
                    if(bytesAvailableToStart == bytesAvailableAfterReadingHeader)
                    {
                        emit d("ERROR: bytesAvailable() doesn't decrease as stream ... err.. streams");
                    }

                    m_SizeExpecting = reinterpret_cast<qint64>(header.messageBodySize);
                    ++m_IntStateKeeperTracker; //or a bool m_AmReadingBody; vs. header, or vice versa
                }
                else
                {
                    //break seems like it'd make sense, but we want to wait for more bytesToBeRead and stream.atEnd won't return true yet... so we return instead
                    return;
                }
            }
            //break; -- we don't break because we want to process the next step
            case 1:
            {
                if(m_ClientSocket->bytesAvailable() > m_SizeExpecting)
                {
                    switch(m_CurrentHeader.MessageType)
                    {
                    case MessageAType:
                        {
                            MessageABody messageBody; //*messageBody = m_Dispenser.getNewOrRecycled();
                            stream >> messageBody;
                            emit d(messageBody.dbgName + QString(" message received from client"));
                        }
                    break;
                    case MessageBType:
                        {
                            MessageBBody messageBody;
                            stream >> messageBody;
                            emit d(messageBody.dbgName + QString(" message received from client"));
                        }
                    break;
                    default:
                        break;
                    }
                    ++m_IntStateKeeperTracker;
                }
                else
                {
                    return;
                }
            }
            case 99999: //at the end, we set m_SizeExpecting back to header size
                {
                    m_SizeExpecting = sizeof(MessageHeader);
                    m_IntStateKeeperTracker = 0;
                }
            break; // we DO break at the very end, so as not to go into default: section
            }
            default:
            break;
        }
    }
#endif
}
void TestServer::sendNextChunkOfBroadcastMessage()
{
}
