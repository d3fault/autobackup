#include "testserver.h"

TestServer::TestServer(QObject *parent) :
    QObject(parent), m_WaitingForHeader(true)
{
    m_CurrentHeader = new MyMessageHeader();
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

    emit d(QString("Bytes Available (start): ") + QString::number(m_ClientSocket->bytesAvailable()));
    QDataStream stream(m_ClientSocket);

#ifdef TYPICAL_SHORT_READING_AND_FAILING
    while(!stream.atEnd())
    {
        emit d(QString("Bytes Available (pre header): ") + QString::number(m_ClientSocket->bytesAvailable()));
        MyMessageHeader header;
        stream >> header;
        emit d(QString("Bytes Available (post header): ") + QString::number(m_ClientSocket->bytesAvailable()));
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
#else
    while(!stream.atEnd())
    {
        qint64 bytesAvailableToStart = m_ClientSocket->bytesAvailable();
        if(bytesAvailableToStart >= (qint64)sizeof(quint32))
        {
            if(m_WaitingForHeader)
            {
                if(bytesAvailableToStart >= (((qint64)(sizeof(MyMessageHeader))) + ((qint64)(sizeof(quint32)))))
                {
                    quint32 magic;
                    stream >> magic;
                    if(magic == (quint32)0xA0B0C0D0)
                    {
                        //MessageHeader header;
                        stream >> *m_CurrentHeader;

                        qint64 bytesAvailableAfterReadingHeader = m_ClientSocket->bytesAvailable();
                        if(bytesAvailableToStart == bytesAvailableAfterReadingHeader)
                        {
                            emit d("ERROR: bytesAvailable() doesn't decrease as stream ... err.. streams");
                        }

                        emit d(QString("server incoming client message body size according to network: ") + QString::number(m_CurrentHeader->MessageSize));
                        m_WaitingForHeader = false;
                    }
                }
                else
                {
                    return;
                }
            }
            else
            {
                if(bytesAvailableToStart >= (qint64)(m_CurrentHeader->MessageSize))
                {
                    switch(m_CurrentHeader->MessageType)
                    {
                    case MyMessageHeader::MessageAType:
                        {
                            MyMessageABody body; //*messageBody = m_Dispenser.getNewOrRecycled();
                            stream >> body;
                            QString commaSpace(", ");
                            emit d(QString::number(body.RandomInt1) + commaSpace + QString::number(body.RandomUInt2) + commaSpace + body.RandomString1 + commaSpace + body.RandomString2);
                        }
                    break;
                    case MyMessageHeader::MessageBType:
                        {
                            MyMessageBBody body;
                            stream >> body;
                            QString commaSpace(", ");
                            emit d(QString::number(body.RandomInt1) + commaSpace + QString::number(body.RandomUInt2) + commaSpace + body.RandomString1 + commaSpace + body.RandomString2);
                        }
                    break;
                    default:
                        break;
                    }
                    m_WaitingForHeader = true;
                }
                else
                {
                    return;
                }
            }
        }
        else
        {
            return;
        }
    }
#endif
}
void TestServer::sendNextChunkOfBroadcastMessage()
{
}
