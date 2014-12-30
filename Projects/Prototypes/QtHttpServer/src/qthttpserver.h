#ifndef QTHTTPSERVER_H
#define QTHTTPSERVER_H

#include <QtNetwork/QTcpServer>

class QtHttpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit QtHttpServer(QObject *parent = 0);
    bool startQtHttpServer(quint16 port);
signals:
    void e(const QString &msg);
    void quitRequested();
private slots:
    void handleNewConnection();
};

#endif // QTHTTPSERVER_H
