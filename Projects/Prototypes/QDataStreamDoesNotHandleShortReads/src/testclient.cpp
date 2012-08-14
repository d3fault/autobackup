#include "testclient.h"

#include "simpleassprotocol.h"

TestClient::TestClient(QObject *parent) :
    QObject(parent), m_CurrentMessageTypeIsA(true), m_MessageSendStage(0)
{
    m_Client = new SslTcpClient(this, ":/ClientCA.pem", ":/ServerCA.pem", ":/ClientPrivateKey.pem", ":/ClientPublicCert.pem", "fuckyou");

    connect(m_Client, SIGNAL(connectedAndEncrypted(QSslSocket*)), this, SLOT(handleConnectedAndEncrypted(QSslSocket*)));
    connect(m_Client, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
void TestClient::init()
{
    if(m_Client->init())
    {
        emit initialized();
    }
}
void TestClient::start()
{
    if(m_Client->start())
    {
        emit started();
    }
}
void TestClient::stop()
{
    m_Client->stop();
    emit stopped();
}
void TestClient::handleConnectedAndEncrypted(QSslSocket *newConnection)
{
    m_ServerSocket = newConnection;
    connect(newConnection, SIGNAL(readyRead()), this, SLOT(handleServerSentUsData()));
    emit d("connected and encrypted to server");
}
void TestClient::handleServerSentUsData()
{
    QSslSocket *socket = static_cast<QSslSocket*>(sender());
    QDataStream stream(socket);
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
                emit d(QString("Type: " + QString::number(header.MessageType) + QString(" message received from server")));
            }
        break;
        case MyMessageHeader::MessageBType:
            {
                MyMessageBBody body;
                stream >> body;
                emit d(QString("Type: " + QString::number(header.MessageType) + QString(" message received from server")));
            }
        break;
        default:
            break;
        }
    }
}
void TestClient::sendNextChunkOfActionRequestMessage()
{
    QDataStream stream(m_ServerSocket);
    switch(m_MessageSendStage)
    {
    case 0:
        {
            emit d("sending stage 0");
            stream << (quint32)0xA0B0C0D0;
            ++m_MessageSendStage;
        }
    //break;
    case 1:
        {
            emit d("sending stage 1");
            stream << ((quint32) (m_CurrentMessageTypeIsA ? MyMessageHeader::MessageAType : MyMessageHeader::MessageBType));
            ++m_MessageSendStage;
        }
    //break;
    case 2:
        {
            emit d("sending stage 2");
            stream << (qint32)3843;
            ++m_MessageSendStage;
        }
    //break;
    case 3:
        {
            emit d("sending stage 3");
            stream << (quint32)923730;
            ++m_MessageSendStage;
        }
    //break;
    case 4:
        {
            emit d("sending stage 4");
            stream << QString(QString("lol fucken first test string @ ") + QDateTime::currentDateTime().toString());
            ++m_MessageSendStage;
        }
    //break;
    case 5:
        {
            emit d("sending stage 5");
            stream << QString("string 2");
            ++m_MessageSendStage;
        }
    //break;
    default:
    {
        emit d("reached end of stages, going back to 0");
        m_MessageSendStage = 0;
        m_CurrentMessageTypeIsA = !m_CurrentMessageTypeIsA; //a becomes b, b becomes a
    }
    //break;
    }
}
