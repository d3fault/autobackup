#ifndef GETTODAYSADSLOTSERVER_H
#define GETTODAYSADSLOTSERVER_H

#include <QObject>

#include <boost/lockfree/queue.hpp>

class QTcpServer;

class GetCouchbaseDocumentByKeyRequest;

class GetTodaysAdSlotServer : public QObject
{
    Q_OBJECT
public:
    static void setBackendQueue(boost::lockfree::queue<GetCouchbaseDocumentByKeyRequest*> *backendQueue);
    static void setBackendQueueEvent(struct event *backendQueueEvent);

    explicit GetTodaysAdSlotServer(QObject *parent = 0);
private:
    QTcpServer *m_Server;
public slots:
    bool initializeAndStart(quint16 port);
    void stopAndDestroy();
private slots:
    void handleNewConnection();
};

#endif // GETTODAYSADSLOTSERVER_H
