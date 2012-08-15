#include "testclient.h"

void TestClient::init()
{
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
}
