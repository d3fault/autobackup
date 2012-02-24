#include "ssltcpserver.h"

SslTcpServer::SslTcpServer(QObject *parent) :
    QTcpServer(parent)
{
}
void SslTcpServer::incomingConnection(int handle)
{
    QSslSocket *secureSocket = new QSslSocket(this);
    if(secureSocket->setSocketDescriptor(handle))
    {
        //connect(secureSocket, SIGNAL(encrypted()), this, SLOT(handleEncrypted()));
        //connect(secureSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(handleSslErrors(QList<QSslError>)));

        //TODO: setPrivateKey and setLocalCertificate
        /*
          openssl genrsa -out privkey.pem 2048
          openssl req -new -x509 -key privkey.pem -out cacert.pem -days 1095

          i should clarify. the above commands work and all, but it's for a self-signed certificate... which is an ssl error in itself.

          the proper way is to use /etc/pki/tls/misc/CA script to generate the CA, the cert/private-key, and then sign the cert with the CA.

          ./CA -newca
          ./CA -newreq
          ./CA -sign

          as simple as that really...
          then put the CA on both the server/client
          and the cert/private-key on the server only
          (unless client has it's own CA, then the process is [additionally] reversed and both sides then have 2 CA certs as well. their own and their peers)

        */

        QFile caFileResource(":/CAcert.pem");
        caFileResource.open(QFile::ReadOnly);
        QByteArray caByteArray = caFileResource.readAll();
        caFileResource.close();

        QSslCertificate certificateAuthority(caByteArray);
        if(certificateAuthority.isNull())
        {
            emit d("the CA is null");
            return;
        }
        emit d("the certificate is not null");

        QList<QSslCertificate> whyForceAListBastards; //list will be nice once the client has it's own CA issuer cert ;-)
        whyForceAListBastards.append(certificateAuthority);
        //QSslSocket::setDefaultCaCertificates(whyForceAListBastards);
        secureSocket->setCaCertificates(whyForceAListBastards);


        QByteArray passPhrase("fuckyou"); //pointless, i shouldn't have entered one in CA script. on the client side (the webserver in my case), however... i COULD use one and prompt for it whenever the application starts up. so the hdd copy of it wouldn't be able to authenticate itself unless _I_ am the one who starts it up. an adversary would have to go through the extra step of extracting the key from memory. i do actually think this is a good extra security step... especially with encrypted memory/paging (encrypted hdd too?? i guess it's not needed but might as well... MIGHT AS WELL? NO MAYNG SHIT SLOWS SHIT DOWN MAYNG)
        secureSocket->setPrivateKey(":/serverprivatekey.pem", QSsl::Rsa, QSsl::Pem, passPhrase);
        secureSocket->setLocalCertificate(":/servercert.pem");
        //secureSocket->setCaCertificates(); <-- i think i need to do this one the client for the ca cert that generated my server's local cert or something? barely understand what i'm doing
        //^^i need to add MY OWN ca cert so that i can verify MY cert against it.. but also the client gets it so it can verify MY cert too. both sides need the CA issuer cert.. but the client only needs the CA cert.
        //^^note, i plan to have the client having it's own private key, cert, and ca cert... so we'll be adding yet more ca certs into the mix (i should rename them appropriately: serverIssuerCAcert etc)... so in the long run, both sides need both CA certs, but each side also has it's own private key and regular cert (that it sends upon request)

        m_PendingConnections.enqueue(secureSocket);
        secureSocket->startServerEncryption();
        emit d("server is initiating delayed handshake");


        //retarded debugging
        /*if(!secureSocket->waitForEncrypted())
        {
            emit d(secureSocket->errorString());
        }
        emit d("got past wait");*/
    }
    else
    {
        emit d("failed to set socket descriptor");
        delete secureSocket;
    }
}
/*void SslTcpServer::handleEncrypted()
{
    emit d("the socket has emitted encrypted()");
    //hopefully this will work...
    QSslSocket *secureSocket = qobject_cast<QSslSocket*>(sender());
    emit newEncryptedConnection(secureSocket);
}*/
QTcpSocket * SslTcpServer::nextPendingConnection()
{
    if(m_PendingConnections.isEmpty())
    {
        return 0;
    }
    return m_PendingConnections.dequeue();
}
