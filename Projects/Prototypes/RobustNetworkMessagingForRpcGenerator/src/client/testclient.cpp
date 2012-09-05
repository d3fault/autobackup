#include "testclient.h"

void TestClient::init()
{
    m_DebugMessageNum = 0;
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
}
void TestClient::handleServerSentUsData()
{
    emit d("server sent us data");

    while(!m_ServerStream->atEnd())
    {
        QString text;
        (*m_ServerStream) >> text;
        emit d(text);
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
