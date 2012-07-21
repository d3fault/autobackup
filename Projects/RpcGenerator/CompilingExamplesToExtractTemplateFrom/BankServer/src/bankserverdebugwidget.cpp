#include "bankserverdebugwidget.h"

bankServerDebugWidget::bankServerDebugWidget(IDebuggableStartableStoppableBankServerBackend *debuggableStartableStoppableBankServerBackend)
    : m_DebuggableStartableStoppableBankServerBackend(debuggableStartableStoppableBankServerBackend)
{
    QMetaObject::invokeMethod(this, "buildGui", Qt::QueuedConnection); //show gui asap
}
void bankServerDebugWidget::buildGui()
{
    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();
    m_StartButton = new QPushButton("Start");
    m_StopButton = new QPushButton("Stop");

    m_Layout->addWidget(m_Debug);
    m_Layout->addWidget(m_StartButton);
    m_Layout->addWidget(m_StopButton);

    this->setLayout(m_Layout);

    m_BackendThread = new QThread();
    m_DebuggableStartableStoppableBankServerBackend->moveToThread(m_BackendThread);
    m_BackendThread->start();

    connect(m_StartButton, SIGNAL(clicked()), m_DebuggableStartableStoppableBankServerBackend, SLOT(start()));
    connect(m_StopButton, SIGNAL(clicked()), m_DebuggableStartableStoppableBankServerBackend, SLOT(stop()));

    connect(m_DebuggableStartableStoppableBankServerBackend, SIGNAL(d(const QString &)), m_Debug, SLOT(appendPlainText(const QString &)));
    connect(m_DebuggableStartableStoppableBankServerBackend, SIGNAL(initialized()), this, SLOT(handleBackendInitialized()));
    connect(m_DebuggableStartableStoppableBankServerBackend, SIGNAL(started()), this, SLOT(handleBackendStarted()));
    connect(m_DebuggableStartableStoppableBankServerBackend, SIGNAL(stopped()), this, SLOT(handleBackendStopped()));

    //now send the "init" message to the backend object
    QMetaObject::invokeMethod(m_DebuggableStartableStoppableBankServerBackend, "init", Qt::QueuedConnection);
}
void bankServerDebugWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void bankServerDebugWidget::handleBackendInitialized()
{
    handleD("GUI received backend initialized signal");
}
void bankServerDebugWidget::handleBackendStarted()
{
    handleD("GUI received backend started signal");
}
void bankServerDebugWidget::handleBackendStopped()
{
    handleD("GUI received backend stopped signal");
}
bankServerDebugWidget::~bankServerDebugWidget()
{

}
