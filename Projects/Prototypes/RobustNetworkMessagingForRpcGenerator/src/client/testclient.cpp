#include "testclient.h"

void TestClient::init()
{
    m_DebugMessageNum = 0;
    m_IncomingMessageSize = 0;
    m_OnFirstMessage = true;
    m_Client = new SslTcpClient(this, ":/ClientCA.pem", ":/ServerCA.pem", ":/ClientPrivateKey.pem", ":/ClientPublicCert.pem", "fuckyou");

    connect(m_Client, SIGNAL(connectedAndEncrypted(QSslSocket*)), this, SLOT(handleConnectedAndEncrypted(QSslSocket*)));
    connect(m_Client, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    if(m_Client->init())
    {
        emit initialized();
    }
    else
    {
        emit d("client failed to initialize");
    }
}
void TestClient::start()
{
    if(m_Client->start())
    {
        emit started();
        emit d("client started");
    }
    else
    {
        emit d("client failed to start");
    }
}
void TestClient::stop()
{
    m_Client->stop();
    emit stopped();
}
void TestClient::handleConnectedAndEncrypted(QSslSocket *socketToServer)
{
    emit d("connected to server");
    connect(socketToServer, SIGNAL(readyRead()), this, SLOT(handleServerSentUsData()));
    m_ServerStream = new QDataStream(socketToServer);

    socketToServer->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    socketToServer->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
}
void TestClient::handleServerSentUsData()
{
    if(m_OnFirstMessage)
    {
        readHelper();
    }
    else
    {
        emit d("we got a readyRead, but we're no longer on the first message so we're going to intentionally ignore it [for now] to see what happens");
    }
}
void TestClient::sendMessageToPeer()
{
    if(m_Client && m_Client->isSslConnectionGood())
    {
        QString blah;
        blah.append(QDateTime::currentDateTime().toString());
        blah.append(QString(" - "));
        blah.append(QString::number(m_DebugMessageNum));
        blah.append(QString("\n"));

        emit d(QString("Sending Message Num: ") + QString::number(m_DebugMessageNum));

        ++m_DebugMessageNum;

        (*m_ServerStream) << blah;
    }
}
void TestClient::readHelper()
{
    emit d("server sent us data");

    /*while(!m_ServerStream->atEnd())
    {
        QString text;
        (*m_ServerStream) >> text;
        emit d(text);
    }*/

    if(m_IncomingMessageSize == 0)
    {
        if(m_Client->bytesAvailable() < (qint64)sizeof(quint16))
        {
            return;
        }
        (*m_ServerStream) >> m_IncomingMessageSize;

        emit d(QString("Incoming Message Size: ") + QString::number(m_IncomingMessageSize));
    }

    if(m_Client->bytesAvailable() < (qint64)m_IncomingMessageSize)
    {
        return;
    }

    QString theMessage;
    (*m_ServerStream) >> theMessage;

    emit d("we read in a message:");
    emit d(theMessage);

    if(m_Client->bytesAvailable() > 0)
    {
        emit d(QString("we still have ") + QString::number(m_Client->bytesAvailable()) + QString(" on the socket to be read..."));
    }

    m_IncomingMessageSize = 0;
    m_OnFirstMessage = false;
}
void TestClient::readSecondMessage()
{
    if(m_OnFirstMessage)
    {
        emit d("lol wut haven't read first message yet?");
    }
    else
    {
        readHelper();
    }
}
