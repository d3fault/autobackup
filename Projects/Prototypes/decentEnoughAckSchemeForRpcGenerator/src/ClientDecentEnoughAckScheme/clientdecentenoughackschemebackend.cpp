#include "clientdecentenoughackschemebackend.h"

ClientDecentEnoughAckSchemeBackend::ClientDecentEnoughAckSchemeBackend(QObject *parent) :
    QObject(parent)
{
    m_Client = new SslTcpClient(this, ":/ClientCA.pem", ":/ServerCA.pem", ":/ClientPrivateKey.pem", ":/ClientPublicCert.pem", "fuckyou");

    connect(m_Client, SIGNAL(connectedAndEncrypted(QSslSocket*)), this, SLOT(handleConnectedAndEncrypted(QSslSocket*)));
    connect(m_Client, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    if(m_Client->init())
    {
        //fuck, can't emit yet because we are in the constructor and haven't necessarily connected to the backend yet -_-. I don't think there _IS_ a solution to the "listen to signals in an object's constructor" problem. I really can't think of any way. Every solution I can think of involves a separate, to be called later (after connecting), init() method. I thought this objectonthreadhelper solution was cool because i didn't need init/destroy -_-. Well I still might not need destroy :-P.. but still. FUCK IT THOUGH MAYNG GAH CARE TOO MUCH ABOUT DESIGN PERFECTION IT WILL DRIVE YOU MAD. You can care about it as much as you want and prematurely optimize your fucking heart out (if that's what you're into) POST-LAUNCH. Right now you're being stupid.

        //emit initialized();
    }
    else
    {
        //emit d("client failed to initialize");
    }
}
void ClientDecentEnoughAckSchemeBackend::startClientBackend()
{
    if(m_Client->start())
    {
        emit d("client started");
    }
    else
    {
        emit d("client failed to start");
    }
}
void ClientDecentEnoughAckSchemeBackend::handleConnectedAndEncrypted(QSslSocket *socketToServer)
{
    emit d("connected to server");
    connect(socketToServer, SIGNAL(readyRead()), this, SLOT(handleServerSentUsData()));
    m_ServerStream = new QDataStream(socketToServer);

    socketToServer->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    socketToServer->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
}
void ClientDecentEnoughAckSchemeBackend::handleServerSentUsData()
{fuck.this->shit
}
void ClientDecentEnoughAckSchemeBackend::stopClientBackend()
{
    m_Client->stop();
}
