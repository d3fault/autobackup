#include "protocolserver.h"

ProtocolServer::ProtocolServer(QObject *parent) :
    QObject(parent), m_TcpServer(0)
{

}
void ProtocolServer::start()
{
    if(!m_TcpServer)
    {
        m_TcpServer = new QTcpServer(this);
        connect(m_TcpServer, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
        m_TcpServer->listen(QHostAddress::LocalHost, 6969);
        emit d("server listening on port 6969");
    }
}
void ProtocolServer::handleNewConnection()
{
    //todo: later, when i do experimenting with QSslSocket, i think I need to modify what happens here
    emit d("new connection");
    if(QTcpSocket *clientConnection = m_TcpServer->nextPendingConnection())
    {
        connect(clientConnection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError()));
        connect(clientConnection, SIGNAL(readyRead()), this, SLOT(handleClientReadyRead()));
        connect(clientConnection, SIGNAL(disconnected()), this, SLOT(handleClientDisconnect()));
    }
}
void ProtocolServer::handleSocketError()
{
    if(QTcpSocket *conn = qobject_cast<QTcpSocket*>(sender()))
    {
        emit d("Error: " + conn->errorString());
    }
}
void ProtocolServer::handleClientReadyRead()
{
    emit d("client ready read");
    if(QTcpSocket *conn = qobject_cast<QTcpSocket*>(sender()))
    {
        QDataStream stream(conn);
        while(!stream.atEnd())
        {
            emit d("top of atEnd while loop");

            MessageFromClient messageFromClient;
            stream >> messageFromClient;
            if(messageFromClient.m_MessageType != Message::MessageFromClientType)
            {
                emit d("somehow received a message that wasn't a MessageFromClientType");
                return;
            }
            switch(messageFromClient.m_MessageSubType)
            {
            case MessageFromClient::HelloMessageFromClientSubType:
                emit d(messageFromClient.m_MessageStringValue);
                emit d(QString::number(messageFromClient.m_MessageIntValue));
                emit d("received hello from client");
                break;
            case MessageFromClient::GoodbyeMessageFromClientSubType:
                emit d("received goodbye from client");
                break;
            case MessageFromClient::InvalidMessageFromClientSubType:
            default:
                emit d("invalid message from client");
                break;
            }
        }
    }
}
void ProtocolServer::handleClientDisconnect()
{
    emit d("tcp socket client disconnected"); //todo, integrate this with the GoodbyeMessage etc? this is all just a test anyways...
}
