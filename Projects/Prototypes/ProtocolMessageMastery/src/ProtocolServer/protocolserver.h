#ifndef PROTOCOLSERVER_H
#define PROTOCOLSERVER_H

#include <QObject>

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include "../Protocol/myprotocol.h"

class ProtocolServer : public QObject
{
    Q_OBJECT
public:
    explicit ProtocolServer(QObject *parent = 0);

signals:
    void d(const QString &);
private:
    QTcpServer *m_TcpServer;

    void sendWelcome(QTcpSocket *conn);
    void sendBeer(QTcpSocket *conn);
    void sendOkTakeCare(QTcpSocket *conn);
public slots:
    void start();
private slots:
    void handleNewConnection();
    void handleSocketError();
    void handleClientReadyRead();
    void handleClientDisconnect();
};

#endif // PROTOCOLSERVER_H
