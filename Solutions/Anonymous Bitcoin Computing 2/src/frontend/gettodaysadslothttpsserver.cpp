#include "gettodaysadslothttpsserver.h"

#include <QFile>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QSslConfiguration>

#include "gettodaysadslotserverclientconnection.h"

GetTodaysAdSlotHttpsServer::GetTodaysAdSlotHttpsServer(QObject *parent)
    : QTcpServer(parent)
{ }
bool GetTodaysAdSlotHttpsServer::initialize(const QString &sslCertFilePath, const QString &sslPrivkeyFilePath)
{
    QFile serverPublicLocalCertificateFileResource(sslCertFilePath);
    if(!serverPublicLocalCertificateFileResource.open(QFile::ReadOnly))
    {
        emit e(sslCertFilePath + " could not be opened (GetTodaysAdSlotHttpsServer)");
        return false;
    }
    QSslCertificate serverPublicLocalCertificate(&serverPublicLocalCertificateFileResource);
    if(serverPublicLocalCertificate.isNull())
    {
        emit e("server public local certificate is null (GetTodaysAdSlotHttpsServer)");
        return false;
    }
    if(serverPublicLocalCertificate.isBlacklisted())
    {
        emit e("server public local certificate is blacklisted (GetTodaysAdSlotHttpsServer)");
        return false;
    }
    QFile serverPrivateEncryptionKeyFileResource(sslPrivkeyFilePath);
    if(!serverPrivateEncryptionKeyFileResource.open(QFile::ReadOnly))
    {
        emit e(sslPrivkeyFilePath + " could not be opened (GetTodaysAdSlotHttpsServer)");
        return false;
    }
    QSslKey serverPrivateEncryptionKey(&serverPrivateEncryptionKeyFileResource, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
    if(serverPrivateEncryptionKey.isNull())
    {
        emit e("server private encryption key is null (GetTodaysAdSlotHttpsServer)");
        return false;
    }

    QSslConfiguration currentSslConfiguration = QSslConfiguration::defaultConfiguration();
    currentSslConfiguration.setLocalCertificate(serverPublicLocalCertificate);
    currentSslConfiguration.setPrivateKey(serverPrivateEncryptionKey);
    QSslConfiguration::setDefaultConfiguration(currentSslConfiguration);

    return true;
}
void GetTodaysAdSlotHttpsServer::incomingConnection(qintptr socketDescriptor)
{
    QSslSocket *clientSocket = new QSslSocket(this);
    if(clientSocket->setSocketDescriptor(socketDescriptor))
    {
        new GetTodaysAdSlotServerClientConnection(clientSocket);
        clientSocket->startServerEncryption();
    }
    else
    {
        delete clientSocket;
    }
}
