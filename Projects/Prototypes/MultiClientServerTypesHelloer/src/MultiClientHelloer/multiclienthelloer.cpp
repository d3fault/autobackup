#include "multiclienthelloer.h"

MultiClientHelloer::MultiClientHelloer(QObject *parent) :
    QObject(parent)
{
}
MultiClientHelloer::~MultiClientHelloer()
{
    //TODOreq: iterate through all connections (pending and active) and delete them? disconnect first? no clue
}
void MultiClientHelloer::addSslClient()
{
    SslTcpClient *newSslClient = new SslTcpClient(this, ":/ClientCA.pem", ":/ServerCA.pem", ":/ClientPrivateKey.pem", ":/ClientPublicCert.pem", "fuckyou");

    connect(newSslClient, SIGNAL(connectedAndEncrypted(QSslSocket*)), this, SLOT(handleClientFinishedInitialConnectPhaseSoStartHelloing(QIODevice*)));
    connect(newSslClient, SIGNAL(d(QString)), this, SIGNAL(d(QString)));


    m_ClientHelloStatusesByIODevice.insert(newSslClient, new ClientHelloStatus());

    if(!newSslClient->init())
    {
        emit d("failed to initialize new ssl client");
        return;
    }
    if(!newSslClient->start())
    {
        emit d("failed to start new ssl client");
        return;
    }
    emit d("new ssl client initialized and started");


}
void MultiClientHelloer::handleClientFinishedInitialConnectPhaseSoStartHelloing(QIODevice *newClient)
{

}
void MultiClientHelloer::handleServerSentUsData()
{
}
