#include "testserver.h"

void TestServer::init()
{
    m_Server = new SslTcpServer(this, ":/ServerCA.pem", ":/ClientCA.pem", ":/ServerPrivateKey.pem", ":/ServerPublicCert.pem", "fuckyou");

    connect(m_Server, SIGNAL(clientConnectedAndEncrypted(QSslSocket*)), this, SLOT(handleClientConnectedAndEncrypted(QSslSocket*)));
    connect(m_Server, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    if(m_Server->init())
    {
        emit initialized();
    }
    else
    {
        emit d("server failed to initialize");
    }
}
void TestServer::start()
{
    if(m_Server->start())
    {
        emit started();
        emit d("server started");
    }
    else
    {
        emit d("server failed to start");
    }
}
void TestServer::stop()
{
    m_Server->stop();
    emit stopped();
}
void TestServer::handleClientConnectedAndEncrypted(QSslSocket *newClientSocket)
{
}
