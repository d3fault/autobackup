#include "mainwidget.h"

mainWidget::mainWidget(QWidget *parent)
    : QWidget(parent)
{
    m_BusinessThread = new QThread();
    m_Business = new RpcServerBusinessImpl();
    m_Business->moveToThread(m_BusinessThread);
    m_BusinessThread->start();

    m_NetworkThread = new QThread();
    m_ClientsHelper = new RpcClientsHelperAndDeliveryAcceptorAndNetwork(m_Business);
    m_ClientsHelper->moveToThread(m_NetworkThread);
    m_NetworkThread->start();

    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();
    m_SimulateActionButton = new QPushButton();

    m_Layout->addWidget(m_Debug);
    m_Layout->addWidget(m_SimulateActionButton);
    this->setLayout(m_Layout);

    connect(m_SimulateActionButton, SIGNAL(clicked()), m_ClientsHelper, SLOT(handleSimulateActionButtonClicked()));

    connect(m_ClientsHelper, SIGNAL(initialized()), m_Business, SLOT(init()));

    QMetaObject::invokeMethod(m_ClientsHelper, "init", Qt::QueuedConnection);
}

mainWidget::~mainWidget()
{

}
