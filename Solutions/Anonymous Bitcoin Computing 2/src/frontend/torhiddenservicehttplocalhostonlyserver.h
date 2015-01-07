#ifndef TORHIDDENSERVICEHTTPLOCALHOSTONLYSERVER_H
#define TORHIDDENSERVICEHTTPLOCALHOSTONLYSERVER_H

#include <QTcpServer>

class TorHiddenServiceHttpLocalhostOnlyServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TorHiddenServiceHttpLocalhostOnlyServer(QObject *parent = 0);
protected:
    void incomingConnection(qintptr socketDescriptor);
};

#endif // TORHIDDENSERVICEHTTPLOCALHOSTONLYSERVER_H
