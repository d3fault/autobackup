#ifndef CLIENTDECENTENOUGHACKSCHEMEBACKEND_H
#define CLIENTDECENTENOUGHACKSCHEMEBACKEND_H

#include <QObject>
#include <QDataStream>

#include "ssltcpclient.h"

class ClientDecentEnoughAckSchemeBackend : public QObject
{
    Q_OBJECT
public:
    explicit ClientDecentEnoughAckSchemeBackend(QObject *parent = 0);
private:
    SslTcpClient *m_Client;
    QDataStream *m_ServerStream;
signals:
    void d(const QString &);
public slots:
    void startClientBackend();
    void handleConnectedAndEncrypted(QSslSocket *socketToServer);
    void handleServerSentUsData();
    void stopClientBackend();
};

#endif // CLIENTDECENTENOUGHACKSCHEMEBACKEND_H
