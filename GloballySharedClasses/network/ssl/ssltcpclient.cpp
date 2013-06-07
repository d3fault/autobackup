#include "ssltcpclient.h"

SslTcpClient::SslTcpClient(QObject *parent)
    : QObject(parent), m_ClientPrivateEncryptionKey(0), m_ClientPublicLocalCertificate(0) /*, const QString &clientCaFile, const QString &serverCaFile, const QString &clientPrivateEncryptionKeyFile, const QString &clientPublicLocalCertificateFile, const QString &clientPrivateEncryptionKeyPassPhrase)
    : QSslSocket(parent), m_ClientCaFile(clientCaFile), m_ServerCaFile(serverCaFile), m_ClientPrivateEncryptionKeyFile(clientPrivateEncryptionKeyFile), m_ClientPrivateEncryptionKeyPassPhrase(clientPrivateEncryptionKeyPassPhrase), m_ClientPublicLocalCertificateFile(clientPublicLocalCertificateFile)*/
{ }
SslTcpClient::~SslTcpClient()
{
    if(this->isOpen())
    {
        this->close();
    }
    if(m_ClientPrivateEncryptionKey)
    {
        delete m_ClientPrivateEncryptionKey;
        m_ClientPrivateEncryptionKey = 0;
    }
    if(m_ClientPublicLocalCertificate)
    {
        delete m_ClientPublicLocalCertificate;
        m_ClientPublicLocalCertificate = 0;
    }
}
void SslTcpClient::initialize(SslTcpClientArgs sslTcpClientArgs)
{
    if(!QSslSocket::supportsSsl())
    {
        emit d("ssl is not supported");
        return;
    }
    m_SslTcpClientArgs = sslTcpClientArgs;

    QFile clientCaFileResource(sslTcpClientArgs.ClientCaFilename);
    clientCaFileResource.open(QFile::ReadOnly);
    QByteArray clientCaByteArray = clientCaFileResource.readAll();
    clientCaFileResource.close();

    QSslCertificate clientCA(clientCaByteArray);
    if(clientCA.isNull())
    {
        emit d("the client CA is null");
        return;
    }
    emit d("the client CA is not null");
    if(!clientCA.isValid())
    {
        emit d("the client CA is not valid");
        return;
    }
    emit d("the client CA is valid");

    QFile serverCaFileResource(sslTcpClientArgs.ServerCaFilename);
    serverCaFileResource.open(QFile::ReadOnly);
    QByteArray serverCaByteArray = serverCaFileResource.readAll();
    serverCaFileResource.close();

    QSslCertificate serverCA(serverCaByteArray);
    if(serverCA.isNull())
    {
        emit d("server CA is null");
        return;
    }
    emit d("server CA is not null");
    if(!serverCA.isValid())
    {
        emit d("server CA is not valid");
        return;
    }
    emit d("server CA is valid");

    m_AllMyCertificateAuthorities.append(clientCA);
    m_AllMyCertificateAuthorities.append(serverCA);

    QByteArray clientPrivateEncryptionKeyPassPhraseByteArray(sslTcpClientArgs.ClientPrivateEncryptionKeyPassPhrase.toUtf8());

    QFile clientPrivateEncryptionKeyFileResource(sslTcpClientArgs.ClientPrivateEncryptionKeyFilename);
    clientPrivateEncryptionKeyFileResource.open(QFile::ReadOnly);
    QByteArray clientPrivateEncryptionKeyByteArray = clientPrivateEncryptionKeyFileResource.readAll();
    clientPrivateEncryptionKeyFileResource.close();

    m_ClientPrivateEncryptionKey = new QSslKey(clientPrivateEncryptionKeyByteArray, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, clientPrivateEncryptionKeyPassPhraseByteArray);

    if(m_ClientPrivateEncryptionKey->isNull())
    {
        emit d("client private encryption key is null");
        return;
    }
    emit d("client private encryption key is not null");

    QFile clientPublicLocalCertificateFileResource(sslTcpClientArgs.ClientPublicLocalCertificateFilename);
    clientPublicLocalCertificateFileResource.open(QFile::ReadOnly);
    QByteArray clientPublicLocalCertificateByteArray = clientPublicLocalCertificateFileResource.readAll();
    clientPublicLocalCertificateFileResource.close();

    m_ClientPublicLocalCertificate = new QSslCertificate(clientPublicLocalCertificateByteArray);
    if(m_ClientPublicLocalCertificate->isNull())
    {
        emit d("client public local certificate is null");
        return;
    }
    emit d("client public local certificate is not null");
    if(!m_ClientPublicLocalCertificate->isValid())
    {
        emit d("client public local certificate is not valid");
        return;
    }
    emit d("client public local certificate is valid");

    sslTcpClientArgs.SslConfiguration.setSslOption(QSsl::SslOptionDisableCompression, true);
    sslTcpClientArgs.SslConfiguration.setCaCertificates(m_AllMyCertificateAuthorities);
    sslTcpClientArgs.SslConfiguration.setPrivateKey(*m_ClientPrivateEncryptionKey);
    sslTcpClientArgs.SslConfiguration.setLocalCertificate(*m_ClientPublicLocalCertificate);

    QSslConfiguration::setDefaultConfiguration(sslTcpClientArgs.SslConfiguration);

#if 0
    //in the server comments i mention conflicts of setting the default configuration. by doing it this way, i am overriding even the default configuration... so those conflicts do not appear. but if i do change this client to use default configuration shit, then they will appear
    this->setCaCertificates(m_AllMyCertificateAuthorities);
    this->setPrivateKey(*m_ClientPrivateEncryptionKey);
    this->setLocalCertificate(*m_ClientPublicLocalCertificate);
    this->setPeerVerifyMode(QSslSocket::VerifyPeer); //perhaps a boolean switch in constructor for whether this is enabled


    //as per CRIME
    QSslConfiguration sslConfiguration = this->sslConfiguration();
    sslConfiguration.setSslOption(QSsl::SslOptionDisableCompression, true);
    this->setSslConfiguration(sslConfiguration);
#endif

    connect(this, SIGNAL(encrypted()), this, SLOT(handleEncrypted()));
    connect(this, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(handleSslErrors(QList<QSslError>)));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError(QAbstractSocket::SocketError)));

    emit d("ssl tcp client initialized");
    return;
}
void SslTcpClient::start()
{
    if(this->state() != QSslSocket::UnconnectedState)
    {
        emit d("ssl tcp client was told to start, but is not in an unconnected state");
        return;
    }

    this->connectToHostEncrypted(m_SslTcpClientArgs.HostAddress, m_SslTcpClientArgs.Port);
    if(this->state() == QSslSocket::UnconnectedState)
    {
        emit d("ssl tcp client connectToHostEncrypted didn't bring us out of UnconnectedState");
        return;
    }

    emit d(QString("ssl tcp client attempting to connect to ") + m_SslTcpClientArgs.HostAddress.toString() + QString(":") + QString::number(m_SslTcpClientArgs.Port));
}
void SslTcpClient::stop()
{
    if(isOpen())
    {
        disconnectFromHost();
        close();
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
#if 0
bool SslTcpClient::isSslConnectionGood()
{
    return ( ( this->isValid() ) && ( this->state() == QAbstractSocket::ConnectedState ) && ( this->isEncrypted() ) );
}
#endif
