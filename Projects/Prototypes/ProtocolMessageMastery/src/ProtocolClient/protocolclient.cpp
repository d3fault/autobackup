#include "protocolclient.h"

ProtocolClient::ProtocolClient(QObject *parent) :
    QObject(parent), m_Socket(0)
{
}
void ProtocolClient::connectToServer()
{
    if(!m_Socket)
    {
        m_Socket = new QTcpSocket(this);

        connect(m_Socket, SIGNAL(connected()), this, SLOT(handleConnected()));
        connect(m_Socket, SIGNAL(readyRead()), this, SLOT(handleServerReadyRead()));
        connect(m_Socket, SIGNAL(disconnected()), this, SLOT(handleDisconnected()));

        m_Socket->connectToHost(QHostAddress::LocalHost, 6969);
        emit d("attempting to connect to server on port 6969");
    }
}
void ProtocolClient::handleConnected()
{
    emit d("connected");

    MessageFromClient helloMessage;
    helloMessage.m_MessageType = Message::MessageFromClientType; //this is redundant
    helloMessage.m_MessageSubType = MessageFromClient::HelloMessageFromClientSubType;
    helloMessage.m_MessageIntValue = 666;
    helloMessage.m_MessageStringValue = QString("what is the point of this during a hello message?");

    QDataStream stream(m_Socket);
    //stream << (*static_cast<const Message*>(&helloMessage)); //this line is fugly. oh wall
    stream << helloMessage;
    emit d("sending hello message to server");
}
void ProtocolClient::handleServerReadyRead()
{
}
void ProtocolClient::handleDisconnected()
{
}
