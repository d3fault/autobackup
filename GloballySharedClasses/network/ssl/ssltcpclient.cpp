#include "ssltcpclient.h"

SslTcpClient::SslTcpClient(QObject *parent, const QString &clientCaFile, const QString &serverCaFile, const QString &clientPrivateEncryptionKeyFile, const QString &clientPublicLocalCertificateFile, const QString &clientPrivateEncryptionKeyPassPhrase)
    : QSslSocket(parent), m_ClientCaFile(clientCaFile), m_ServerCaFile(serverCaFile), m_ClientPrivateEncryptionKeyFile(clientPrivateEncryptionKeyFile), m_ClientPrivateEncryptionKeyPassPhrase(clientPrivateEncryptionKeyPassPhrase), m_ClientPublicLocalCertificateFile(clientPublicLocalCertificateFile)
{ }
bool SslTcpClient::init()
{
    if(!QSslSocket::supportsSsl())
    {
        emit d("ssl is not supported");
        return false;
    }

    QFile clientCaFileResource(m_ClientCaFile);
    clientCaFileResource.open(QFile::ReadOnly);
    QByteArray clientCaByteArray = clientCaFileResource.readAll();
    clientCaFileResource.close();

    QSslCertificate clientCA(clientCaByteArray);
    if(clientCA.isNull())
    {
        emit d("the client CA is null");
        return false;
    }
    emit d("the client CA is not null");
    if(!clientCA.isValid())
    {
        emit d("the client CA is not valid");
        return false;
    }
    emit d("the client CA is valid");

    QFile serverCaFileResource(m_ServerCaFile);
    serverCaFileResource.open(QFile::ReadOnly);
    QByteArray serverCaByteArray = serverCaFileResource.readAll();
    serverCaFileResource.close();

    QSslCertificate serverCA(serverCaByteArray);
    if(serverCA.isNull())
    {
        emit d("server CA is null");
        return false;
    }
    emit d("server CA is not null");
    if(!serverCA.isValid())
    {
        emit d("server CA is not valid");
        return false;
    }
    emit d("server CA is valid");

    m_AllMyCertificateAuthorities.append(clientCA);
    m_AllMyCertificateAuthorities.append(serverCA);


    QByteArray clientPrivateEncryptionKeyPassPhraseByteArray(m_ClientPrivateEncryptionKeyPassPhrase.toUtf8());

    QFile clientPrivateEncryptionKeyFileResource(m_ClientPrivateEncryptionKeyFile);
    clientPrivateEncryptionKeyFileResource.open(QFile::ReadOnly);
    QByteArray clientPrivateEncryptionKeyByteArray = clientPrivateEncryptionKeyFileResource.readAll();
    clientPrivateEncryptionKeyFileResource.close();

    m_ClientPrivateEncryptionKey = new QSslKey(clientPrivateEncryptionKeyByteArray, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, clientPrivateEncryptionKeyPassPhraseByteArray);

    if(m_ClientPrivateEncryptionKey->isNull())
    {
        emit d("client private encryption key is null");
        return false;
    }
    emit d("client private encryption key is not null");


    QFile clientPublicLocalCertificateFileResource(m_ClientPublicLocalCertificateFile);
    clientPublicLocalCertificateFileResource.open(QFile::ReadOnly);
    QByteArray clientPublicLocalCertificateByteArray = clientPublicLocalCertificateFileResource.readAll();
    clientPublicLocalCertificateFileResource.close();

    m_ClientPublicLocalCertificate = new QSslCertificate(clientPublicLocalCertificateByteArray);
    if(m_ClientPublicLocalCertificate->isNull())
    {
        emit d("client public local certificate is null");
        return false;
    }
    emit d("client public local certificate is not null");
    if(!m_ClientPublicLocalCertificate->isValid())
    {
        emit d("client public local certificate is not valid");
        return false;
    }
    emit d("client public local certificate is valid");


    this->setCaCertificates(m_AllMyCertificateAuthorities);
    this->setPrivateKey(*m_ClientPrivateEncryptionKey);
    this->setLocalCertificate(*m_ClientPublicLocalCertificate);
    this->setPeerVerifyMode(QSslSocket::VerifyPeer);


    connect(this, SIGNAL(encrypted()), this, SLOT(handleEncrypted()));
    connect(this, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(handleSslErrors(QList<QSslError>)));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError(QAbstractSocket::SocketError)));

    return true;
}
bool SslTcpClient::start()
{
    if(this->state() == QSslSocket::UnconnectedState)
    {
        this->connectToHostEncrypted("localhost", 6969);
        if(this->state() != QSslSocket::UnconnectedState)
        {
            emit d("attempting to connect to host encrypted on port 6969");
            return true;
        }
    }
    emit d("failed to start connecting to host. already connected?");
    return false;
}
void SslTcpClient::stop()
{
    if(this->isSslConnectionGood())
    {
        this->disconnectFromHost();
        this->close();
    }
    emit d("stopping ssl tcp client");
}
void SslTcpClient::handleEncrypted()
{
    emit d("connection ENCRYPTED");
    QSslSocket *secureSocket = qobject_cast<QSslSocket*>(sender());
    if(secureSocket)
    {
        emit connectedAndEncrypted(secureSocket);
    }
}
void SslTcpClient::handleSslErrors(QList<QSslError> sslErrors)
{
    int numErrors = sslErrors.count();
    emit d(QString::number(numErrors) + " ssl errors");
    for(int i = 0; i < numErrors; ++i)
    {
        emit d("ssl error #" + QString::number(i) + " - " + sslErrors.at(i).errorString());
    }
}
void SslTcpClient::handleSocketError(QAbstractSocket::SocketError abstractSocketError)
{
    emit d("abstract socket error #" + QString::number(abstractSocketError));
}
bool SslTcpClient::isSslConnectionGood()
{
    return ( ( this->isValid() ) && ( this->state() == QAbstractSocket::ConnectedState ) && ( this->isEncrypted() ) );
}
