#ifndef GETTODAYSADSLOTHTTPSSERVER_H
#define GETTODAYSADSLOTHTTPSSERVER_H

#include <QTcpServer>

class GetTodaysAdSlotHttpsServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit GetTodaysAdSlotHttpsServer(QObject *parent = 0);
    virtual ~GetTodaysAdSlotHttpsServer() { }
    bool initialize(const QString &sslCertFilePath, const QString &sslPrivkeyFilePath);
protected:
    void incomingConnection(qintptr socketDescriptor);
signals:
    void e(const QString &msg);
};

#endif // GETTODAYSADSLOTHTTPSSERVER_H
