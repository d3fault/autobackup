#include "mainwidget.h"

mainWidget::mainWidget(RobustNetworkMessagingForRpcGeneratorTestServer *debuggableStartableStoppableBackend, QWidget *parent)
    : IDebuggableStartableStoppableFrontend(debuggableStartableStoppableBackend, parent)
{ }
void mainWidget::addButtonsToLayoutAndConnectToBackend()
{
    m_MsDelayLineEdit = new QLineEdit(QString("100"));
    m_StartSendingMessagesButton = new QPushButton(QString("Start Sending Messages Every n milliseconds"));
    m_StopSendingMessagesButton = new QPushButton(QString("Stop Sending Messages"));

    m_Layout->addWidget(m_MsDelayLineEdit);
    m_Layout->addWidget(m_StartSendingMessagesButton);
    m_Layout->addWidget(m_StopSendingMessagesButton);

    m_MessageSendTimer = new QTimer();
    connect(m_MessageSendTimer, SIGNAL(timeout()), this, SLOT(handleMessageSendTimerTimedOut()));

    connect(m_StartSendingMessagesButton, SIGNAL(clicked()), this, SLOT(toggleSendingMessages()));
    connect(m_StopSendingMessagesButton, SIGNAL(clicked()), this, SLOT(toggleSendingMessages()));
}
void mainWidget::toggleSendingMessages()
{
    if(!m_MessageSendTimer->isActive())
    {
        m_MessageSendTimer->start(m_MsDelayLineEdit->text().toInt());
    }
    else
    {
        m_MessageSendTimer->stop();
    }
}
void mainWidget::handleMessageSendTimerTimedOut()
{
    QMetaObject::invokeMethod(static_cast<RobustNetworkMessagingForRpcGeneratorTestServer*>(m_DebuggableStartableStoppableBackend), "sendMessageToPeer", Qt::QueuedConnection);
}
