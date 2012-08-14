#ifndef DATASTREAMDOESNOTHANDLESHORTREADSBACKEND_H
#define DATASTREAMDOESNOTHANDLESHORTREADSBACKEND_H

#include <QObject>
#include <QThread>

#include "idebuggablestartablestoppablebackend.h"
#include "testserver.h"
#include "testclient.h"

class DataStreamDoesNotHandleShortReadsBackend : public IDebuggableStartableStoppableBackend
{
    Q_OBJECT
public:
    explicit DataStreamDoesNotHandleShortReadsBackend();
public slots:
    void init();
    void start();
    void stop();

    void sendNextChunkOfMessage();
private:
    QThread *m_ServerThread;
    TestServer *m_Server;

    QThread *m_ClientThread;
    TestClient *m_Client;
};

#endif // DATASTREAMDOESNOTHANDLESHORTREADSBACKEND_H
