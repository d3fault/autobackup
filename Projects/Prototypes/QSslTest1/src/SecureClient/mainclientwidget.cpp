#include "mainclientwidget.h"

mainClientWidget::mainClientWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();
    m_StartButton = new QPushButton("Connect");

    m_Layout->addWidget(m_Debug);
    m_Layout->addWidget(m_StartButton);
    this->setLayout(m_Layout);

    m_WorkerThread = new QThread();
    m_SecureClient = new SecureClient();
    m_SecureClient->moveToThread(m_WorkerThread);
    m_WorkerThread->start();

    connect(m_StartButton, SIGNAL(clicked()), m_SecureClient, SLOT(connectToSecureServer()));
    connect(m_SecureClient, SIGNAL(d(QString)), m_Debug, SLOT(appendPlainText(QString)));
}

mainClientWidget::~mainClientWidget()
{

}
