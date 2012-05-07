#ifndef CLIENTSHELPER_H
#define CLIENTSHELPER_H

#include <QObject>

#include "../shared/ssltcpserver.h"

class ClientsHelper : public QObject
{
    Q_OBJECT
public:
    explicit ClientsHelper(QObject *parent = 0);
private:
    SslTcpServer *m_SslTcpServer;
signals:
    void initialized(); //todo: who will listen to this? bank could before going into 'all systems go' mode... but meh seems worthless
    void d(const QString &);
public slots:
    void init();
private slots:
    void handleClientConnectedAndEncrypted(QSslSocket*);
    void handleClientSentUsData();
};

#endif // CLIENTSHELPER_H
