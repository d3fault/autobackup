#include "idebuggablestartablestoppablefrontend.h"

#include "idebuggablestartablestoppablebackend.h"

IDebuggableStartableStoppableFrontend::IDebuggableStartableStoppableFrontend(IDebuggableStartableStoppableBackend *debuggableStartableStoppableFrontend, QWidget *parent)
    : QWidget(parent), m_DebuggableStartableStoppableBackend(debuggableStartableStoppableFrontend)
{
    QMetaObject::invokeMethod(this, "setupGuiAndThenBeginBackendInit", Qt::QueuedConnection); //this guarantees that the object will be done constructing (and the pure virtual will have been implemented). it doubles as a way of making the gui load faster :)
}
void IDebuggableStartableStoppableFrontend::setupGuiAndThenBeginBackendInit()
{
    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();
    m_StartButton = new QPushButton("Start");
    m_StopButton = new QPushButton("Stop");

    m_Layout->addWidget(m_Debug);
    m_Layout->addWidget(m_StartButton);
    m_Layout->addWidget(m_StopButton);

    addButtonsToLayoutAndConnectToBackend();

    this->setLayout(m_Layout);

    connect(m_StartButton, SIGNAL(clicked()), m_DebuggableStartableStoppableBackend, SLOT(start()));
    connect(m_StopButton, SIGNAL(clicked()), m_DebuggableStartableStoppableBackend, SLOT(stop()));

    connect(m_DebuggableStartableStoppableBackend, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
    connect(m_DebuggableStartableStoppableBackend, SIGNAL(initialized()), this, SLOT(handleBackendInitialized()));
    connect(m_DebuggableStartableStoppableBackend, SIGNAL(started()), this, SLOT(handleBackendStarted()));
    connect(m_DebuggableStartableStoppableBackend, SIGNAL(stopped()), this, SLOT(handleBackendStopped()));

    m_BackendThread = new QThread();
    m_DebuggableStartableStoppableBackend->moveToThread(m_BackendThread);
    m_BackendThread->start();

    QMetaObject::invokeMethod(m_DebuggableStartableStoppableBackend, "init", Qt::QueuedConnection);
}
void IDebuggableStartableStoppableFrontend::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void IDebuggableStartableStoppableFrontend::handleBackendInitialized()
{
    handleD("GUI Received Initialized Signal");
}
void IDebuggableStartableStoppableFrontend::handleBackendStarted()
{
    handleD("GUI Received Started Signal");
}
void IDebuggableStartableStoppableFrontend::handleBackendStopped()
{
    handleD("GUI Received Stopped Signal");
}
