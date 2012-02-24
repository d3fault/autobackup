#include "secureserver.h"

SecureServer::SecureServer(QObject *parent) :
    QObject(parent), m_SslTcpServer(0)
{
}
void SecureServer::startServer()
{
    if(!QSslSocket::supportsSsl())
    {
        emit d("ssl is not supported");
        return;
    }
    if(!m_SslTcpServer)
    {
        m_SslTcpServer = new SslTcpServer(this);

        connect(m_SslTcpServer, SIGNAL(newConnection()), this, SLOT(handleConnectedButNotEncryptedYet()));
        //connect(m_SslTcpServer, SIGNAL(newEncryptedConnection(QSslSocket*)), this, SLOT(handleConnectedAndEncrypted(QSslSocket*)));
        connect(m_SslTcpServer, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

        m_SslTcpServer->listen(QHostAddress::LocalHost, 6969);

        emit d("server listening on port 6969");
    }
}
void SecureServer::handleConnectedButNotEncryptedYet()
{
    emit d("new connection, not yet encrypted");
    QSslSocket *newConnection = qobject_cast<QSslSocket*>(m_SslTcpServer->nextPendingConnection()); //is this necessary? we don't care about the unencrypted tcp socket (what it returns)... but do i need to call this to keep the pending connections queue cleared?
    if(newConnection)
    {        
        connect(newConnection, SIGNAL(encrypted()), this, SLOT(handleConnectedAndEncrypted()));
        connect(newConnection, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(handleSslErrors(QList<QSslError>)));
        connect(newConnection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError(QAbstractSocket::SocketError)));
        emit d("got a new connection from nextPendingConnection()");
    }
    else
    {
        emit d("nextPendingConnection() returned 0");
    }
}
void SecureServer::handleConnectedAndEncrypted()
{
    emit d("connection ENCRYPTED");
    QSslSocket *secureSocket = qobject_cast<QSslSocket*>(sender());

    connect(secureSocket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
}
void SecureServer::handleReadyRead()
{
    emit d("received data from client");
    if(QSslSocket *secureSocket = qobject_cast<QSslSocket*>(sender()))
    {
        QDataStream stream(secureSocket);
        while(!stream.atEnd())
        {
            ClientToServerMessage message;
            stream >> message;
            if(message.m_MessageType != Message::ClientToServerMessageType)
            {
                emit d("somehow got the wrong message type");
                return;
            }
            switch(message.m_TheMessage)
            {
            case ClientToServerMessage::HelloSuckMyDick:
                emit d("hello received from client");
                //sendWelcome(secureSocket);
                sendMessageToClient(ServerToClientMessage::WelcomeAndNoThanks, secureSocket);
                break;
            case ClientToServerMessage::GoodbyeGoFuckYourself:
                emit d("goodbye received from client");
                //i should send a ThatsRudeByeNow here... but idk about this race condition stuff...
                secureSocket->disconnectFromHost(); //since we don't send anything back like "ok take care", there is no race condition here
                secureSocket->deleteLater();
                return;
            default:
                emit d("invalid client2server message");
                return;
            }
        }
    }
}
void SecureServer::handleSslErrors(QList<QSslError> sslErrors)
{
    int numErrors = sslErrors.count();
    emit d(QString::number(numErrors) + " ssl errors");
    for(int i = 0; i < numErrors; ++i)
    {
        emit d("ssl error #" + QString::number(i) + " - " + sslErrors.at(i).errorString());
    }
}
void SecureServer::sendMessageToClient(ServerToClientMessage::TheMessage theMessage, QSslSocket *conn)
{
    QDataStream stream(conn);
    ServerToClientMessage message;
    message.m_TheMessage = theMessage;
    stream << message;
}
void SecureServer::handleSocketError(QAbstractSocket::SocketError socketError)
{
    emit d("abstract socket error #" + QString::number(socketError));
}
/*void SecureServer::sendWelcome(QSslSocket *conn)
{
    sendMessageToClient(ServerToClientMessage::WelcomeAndNoThanks, conn);
}*/
