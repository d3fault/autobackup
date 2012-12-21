#ifndef MULTISERVERABSTRACTION_H
#define MULTISERVERABSTRACTION_H

#include <QObject>
#include <QHash>

#include "network/ssl/ssltcpserver.h"

struct MultiServerAbstractionArgs
{
    MultiServerAbstractionArgs() { m_SslEnabled = false; m_TcpEnabled = false; m_LocalServerEnabled = false; }

    //SSL
    SslTcpServerArgs m_SslTcpServerArgs;
    bool m_SslEnabled;

    //TCP
    QHostAddress m_TcpHostAddress;
    quint16 m_TcpPort;
    bool m_TcpEnabled;

    //LOCALSERVER
    QString m_LocalServerName;
    bool m_LocalServerEnabled;
};
class MultiServerAbstraction : public QObject
{
    Q_OBJECT
public:
    explicit MultiServerAbstraction(QObject *parent = 0);
private:
    bool m_BeSslServer, m_BeTcpServer, m_BeLocalServer;
    SslTcpServer *m_SslTcpServer;
    inline void deletePointersAndSetEachFalse() { delete m_SslTcpServer; /* TODOreq: delete Tcp/Local */ m_BeSslServer = false; m_BeTcpServer = false; m_BeLocalServer = false; }
signals:
    void d(const QString &);
public slots:
    void initialize(MultiServerAbstractionArgs multiServerAbstractionArgs);
    void start();

    void handleNewClientConnected(QIODevice *newClient);
    void handleNewClientSentData();
};

#endif // MULTISERVERABSTRACTION_H
