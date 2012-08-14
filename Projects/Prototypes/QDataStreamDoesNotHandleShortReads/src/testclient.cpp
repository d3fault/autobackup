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
    streamOutHelper(m_ServerSocket, m_MessageSendStage);
    m_MessageSendStage++;
    if(m_MessageSendStage > 6)
    {
        m_MessageSendStage = 0;
    }
}
void TestClient::streamOutHelper(QIODevice *device, int stage)
{
    QDataStream stream(device);

    if(stage == 0 || stage == -1)
    {
        emit d("sending stage 0");
        stream << (quint32)0xA0B0C0D0;
    }
    if(stage == 1 || stage == -1)
    {
        emit d("sending stage 1");
        if(stage == 1)
        {
            QByteArray *arrayForSizing = new QByteArray();
            QBuffer *buffer = new QBuffer(arrayForSizing);
            buffer->open(QIODevice::ReadWrite);
            streamOutHelper(buffer, -1);
            quint32 sizeWritten = (quint32)(arrayForSizing->size());
            stream << sizeWritten;
            emit d(QString("Message Size: ") + QString::number(sizeWritten));
            buffer->close();
            delete buffer;
            delete arrayForSizing;
        }
        else
        {
            stream << (quint32)12345; //not needed, just need to stream something to count the size of the size when CALCULATING* the [actual] size
        }
    }
    if(stage == 2 || stage == -1)
    {
        emit d("sending stage 2");
        stream << ((quint32) (m_CurrentMessageTypeIsA ? MyMessageHeader::MessageAType : MyMessageHeader::MessageBType));
    }
    if(stage == 3 || stage == -1)
    {
        emit d("sending stage 3");
        stream << (qint32)3843;
    }
    if(stage == 4 || stage == -1)
    {
        emit d("sending stage 4");
        stream << (quint32)923730;
    }
    if(stage == 5 || stage == -1)
    {
        emit d("sending stage 5");
        stream << QString("lol fucken first test string");
    }
    if(stage == 6 || stage == -1)
    {
        emit d("sending stage 6");
        stream << QString("string 2");
    }
}
