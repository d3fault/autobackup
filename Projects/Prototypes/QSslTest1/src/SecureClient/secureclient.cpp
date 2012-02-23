#include "secureclient.h"

SecureClient::SecureClient(QObject *parent) :
    QObject(parent), m_SslSocket(0)
{
}
void SecureClient::connectToSecureServer()
{
    if(!m_SslSocket)
    {
        m_SslSocket = new QSslSocket(this);

        connect(m_SslSocket, SIGNAL(connected()), this, SLOT(handleConnectedNotYetEncrypted()));
        connect(m_SslSocket, SIGNAL(encrypted()), this, SLOT(handleConnectedAndEncrypted()));
        connect(m_SslSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(handleSslErrors(QList<QSslError>)));
        connect(m_SslSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));

        m_SslSocket->connectToHostEncrypted("127.0.0.1", 6969);
        emit d("attempting to connect at port 6969");

        //retarded DEBUGGING
        if(!m_SslSocket->waitForEncrypted())
        {
            emit d(m_SslSocket->errorString());
        }
    }
}
void SecureClient::handleConnectedNotYetEncrypted()
{
    emit d("connected but not yet encrypted");
}
void SecureClient::handleConnectedAndEncrypted()
{
    emit d("ENCRYPTED");
    connect(m_SslSocket, SIGNAL(readyRead()), this, SLOT(handleReadyRead())); //maybe i should do this sooner? but then again we don't want to process jack shit until we ARE encrypted/verified...
    sendMessageToServer(ClientToServerMessage::HelloSuckMyDick);
}
void SecureClient::handleSslErrors(QList<QSslError> sslErrors)
{
    int numErrors = sslErrors.count();
    emit d(QString::number(numErrors) + " ssl errors");
    for(int i = 0; i < numErrors; ++i)
    {
        emit d("ssl error #" + QString::number(i) + " - " + sslErrors.at(i).errorString());
    }
}
void SecureClient::handleDisconnect()
{
    emit d("disconnected");
}
void SecureClient::handleReadyRead()
{
    emit d("received data from server");
    QDataStream stream(m_SslSocket);
    while(!stream.atEnd())
    {
        ServerToClientMessage message;
        stream >> message;
        if(message.m_MessageType != Message::ServerToClientMessageType)
        {
            emit d("somehow got a message that wasn't a server2client message");
        }
        switch(message.m_TheMessage)
        {
        case ServerToClientMessage::WelcomeAndNoThanks:
            emit d("server said welcome");
            //do-stuff, send other messages etc...
            break;
        case ServerToClientMessage::ThatsRudeByeNow:
            emit d("server acknowledged our bye");
            m_SslSocket->disconnectFromHost();
            m_SslSocket->deleteLater();
            return;
        default:
            emit d("received invalid server2client message");
            return;
        }
    }
}
void SecureClient::sendMessageToServer(ClientToServerMessage::TheMessage theMessage)
{
    QDataStream stream(m_SslSocket);
    ClientToServerMessage message;
    message.m_TheMessage = theMessage;
    stream << message;
}
