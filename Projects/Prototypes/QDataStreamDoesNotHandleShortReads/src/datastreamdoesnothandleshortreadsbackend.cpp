#include "datastreamdoesnothandleshortreadsbackend.h"

DataStreamDoesNotHandleShortReadsBackend::DataStreamDoesNotHandleShortReadsBackend()
{
    m_ServerThread = new QThread();
    m_Server = new TestServer();
    m_Server->moveToThread(m_ServerThread);
    m_ServerThread->start();

    m_ClientThread = new QThread();
    m_Client = new TestClient();
    m_Client->moveToThread(m_ClientThread);
    m_ClientThread->start();

    connect(m_Server, SIGNAL(initialized()), m_Client, SLOT(init()));
    connect(m_Client, SIGNAL(initialized()), this, SIGNAL(initialized()));

    connect(m_Server, SIGNAL(started()), m_Client, SLOT(start()));
    connect(m_Client, SIGNAL(started()), this, SIGNAL(started()));

    connect(m_Client, SIGNAL(stopped()), m_Server, SLOT(stop()));
    connect(m_Server, SIGNAL(stopped()), this, SIGNAL(stopped()));

    connect(m_Client, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(m_Server, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
void DataStreamDoesNotHandleShortReadsBackend::init()
{
    QMetaObject::invokeMethod(m_Server, "init", Qt::QueuedConnection);
    emit d("trying to init");
}
void DataStreamDoesNotHandleShortReadsBackend::start()
{
    QMetaObject::invokeMethod(m_Server, "start", Qt::QueuedConnection);
    emit d("trying to start");
}
void DataStreamDoesNotHandleShortReadsBackend::stop()
{
    QMetaObject::invokeMethod(m_Client, "stop", Qt::QueuedConnection);
    emit d("trying to stop");
}
void DataStreamDoesNotHandleShortReadsBackend::sendNextChunkOfMessage()
{
    //send "sendNextChunkOfActionRequestMessage" to client
    QMetaObject::invokeMethod(m_Client, "sendNextChunkOfActionRequestMessage", Qt::QueuedConnection);

    //send "sendNextChunkOfBroadcastMessage" to server
    QMetaObject::invokeMethod(m_Server, "sendNextChunkOfBroadcastMessage", Qt::QueuedConnection);
}
