#include "appclienthelper.h"

AppClientHelper::AppClientHelper(QObject *parent) :
    QObject(parent), m_SslTcpServer(0)
{
}
void AppClientHelper::startListening()
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
        connect(m_SslTcpServer, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

        m_SslTcpServer->listen(QHostAddress::LocalHost, 6969);

        emit d("server listening on port 6969");
    }
}
void AppClientHelper::handleConnectedButNotEncryptedYet()
{
    emit d("new connection, not yet encrypted");
    QSslSocket *newConnection = qobject_cast<QSslSocket*>(m_SslTcpServer->nextPendingConnection());
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
void AppClientHelper::handleConnectedAndEncrypted()
{
    emit d("connection ENCRYPTED");
    QSslSocket *secureSocket = qobject_cast<QSslSocket*>(sender());
    if(secureSocket)
    {
        connect(secureSocket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));

        //it makes more sense, from a design perspective, to put the socket in m_SocketsByAppId right here. unfortunately we don't have appId yet, we've only just established an encrypted connection. of there was a handleAppIdMessage, that would be an even better place. for now, we're going to put it in our readyReady and INEFFICIENTLY check it using .contains(appId) for every message. if not exist, add it
    }
}
void AppClientHelper::handleReadyRead()
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

            //TODO: check app_id against a list of acceptable app_id's
            if(message.m_AppId != "lkjfosidf08043298234098234dsafljkd")
            {
                emit d("invalid app id from client");
                return;
            }

            //hack, should really be in a handleAppIdMessage function, which requires redesigning the client side (serverhelper)
            if(!m_SocketsByAppId.contains(message.m_AppId))
            {
                m_SocketsByAppId.insert(message.m_AppId, secureSocket); //we put it in here so we can get it later when we want to send a message back to the client. see handleUserAdded for an example
            }

            switch(message.m_TheMessage)
            {
            case ClientToServerMessage::AddUserWithThisName:
                emit addUserRequested(message.m_AppId, message.m_ExtraString /*username*/);
                //definitely thread separated message post here.. the other thread will be polling bitcoin... we never want to block the tcp server
                break;
            default:
                emit d("invalid client2server message");
                return;
            }
        }
    }
}
void AppClientHelper::handleSslErrors(QList<QSslError> sslErrors)
{
    int numErrors = sslErrors.count();
    emit d(QString::number(numErrors) + " ssl errors");
    for(int i = 0; i < numErrors; ++i)
    {
        emit d("ssl error #" + QString::number(i) + " - " + sslErrors.at(i).errorString());
    }
}
void AppClientHelper::handleSocketError(QAbstractSocket::SocketError socketError)
{
    emit d("abstract socket error #" + QString::number(socketError));
}
void AppClientHelper::handleUserAdded(const QString &appId, const QString &userName)
{
    //so eh at this point i guess we have to make sure that we're going to send the message to the right appId. the right ssl socket, that is. i guess the easiest way to go about this is to store a QHash<QString appId, QSslSocket *associatedConnection> earlier and read from it here/now. i think this touches on what i was considering in the client... whether to only send the app id once or with every message. the answer APPEARS to be (so far) that i can just receive it once and keep my QHash... i think i can look up the key (appId) by value (QSslSocket) too... so we just find the key for the sender() value at an onReadyRead and bam we have our appId. if QHash doesn't support that functionality i _KNOW_ something else does. still, the rewrite of code necessary to block/wait for appIdOk doesn't seem worth it so for now, fuck it. we're even going to send the appId BACK... for extra verification

    //TODO: we add our appId to the hash of connections no matter what, but if Bank propogates back to us that appId was invalid, we should do something with that information... flag the connection as malicious... or just close it and block the IP. idfk.

    //i could put this in a sendUserAddedMessage() function, but i see no point atm. hell, my use of sendAddUserMessage() in the client code (serverhelper) is pointless too..
    ServerToClientMessage message(appId, ServerToClientMessage::UserAdded, userName);
    QSslSocket *appSocket = getSocketByAppId(appId);
    if(isConnected(appSocket))
    {
        QDataStream stream(appSocket);
        stream << message;
    }
    else
    {
        emit d("unable to send user added message. connection lost?");
        //todo: idfk what to do when the client loses connection. maybe queue up the messages and send them later when they reconnect????
        //they are all struct Messages so a queue for an appId doesn't sound too hard to do
        //we check for the existence of said queue/cache on appId connection and flush it
        //an app id specific message would be perfect for this... otherwise we're waiting for an action... which might lead us to go out of sync or god knows what else. at the very least it'll cause us to wait for an action to occur... instead of flushing the queue right when they connect (as we should)
        //todo: LOL, the queue should be a db-backed one... what if so many messages get queued that we run out of memory? every hypothetical scenario must be accounted for. fml.
    }
}
QSslSocket * AppClientHelper::getSocketByAppId(const QString &appId)
{
    //return the socket pointer or 0
    return m_SocketsByAppId.value(appId, 0);
}
bool AppClientHelper::isConnected(QSslSocket *socketToCheck)
{
    if(socketToCheck /*getSocketByAppId returns 0 on error*/ && socketToCheck->isValid() && socketToCheck->isEncrypted())
    {
        return true;
    }
    return false;
}
