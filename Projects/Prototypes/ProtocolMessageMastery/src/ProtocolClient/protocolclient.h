#ifndef PROTOCOLCLIENT_H
#define PROTOCOLCLIENT_H

#include <QObject>

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>

#include "../Protocol/myprotocol.h"

class ProtocolClient : public QObject
{
    Q_OBJECT
public:
    explicit ProtocolClient(QObject *parent = 0);
private:
    QTcpSocket *m_Socket;
signals:
    void d(const QString &);
public slots:
    void connectToServer();
private slots:
    void handleConnected();
    void handleServerReadyRead();
    void handleDisconnected();
};

#endif // PROTOCOLCLIENT_H
